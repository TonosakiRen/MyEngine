#include "Model.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "CommandContext.h"
#include "ShaderManager.h"
#include "SamplerManager.h"

std::unique_ptr<RootSignature> Model::rootSignature_;
std::unique_ptr<PipelineState> Model::pipelineState_;

void Model::CreatePipeline(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat) {
    rootSignature_ = std::make_unique<RootSignature>();
    pipelineState_ = std::make_unique<PipelineState>();

    {
        CD3DX12_DESCRIPTOR_RANGE ranges[1]{};
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        CD3DX12_DESCRIPTOR_RANGE samplerRanges[1]{};
        samplerRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

        CD3DX12_ROOT_PARAMETER rootParameters[4]{};
        rootParameters[0].InitAsConstantBufferView(0);
        rootParameters[1].InitAsConstantBufferView(1);
        rootParameters[2].InitAsDescriptorTable(_countof(ranges), ranges);
        rootParameters[3].InitAsDescriptorTable(_countof(samplerRanges), samplerRanges);

        /*  CD3DX12_STATIC_SAMPLER_DESC staticSampler(
              0,
              D3D12_FILTER_MIN_MAG_MIP_POINT);*/

        D3D12_ROOT_SIGNATURE_DESC desc{};
        desc.pParameters = rootParameters;
        desc.NumParameters = _countof(rootParameters);
        /* desc.pStaticSamplers = &staticSampler;
         desc.NumStaticSamplers = 1;*/
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        rootSignature_->Create(L"Model RootSignature", desc);
    }

    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
        auto shaderManager = ShaderManager::GetInstance();

        desc.pRootSignature = *rootSignature_;

        D3D12_INPUT_ELEMENT_DESC inputElements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };
        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
        inputLayoutDesc.pInputElementDescs = inputElements;
        inputLayoutDesc.NumElements = _countof(inputElements);
        desc.InputLayout = inputLayoutDesc;

        auto vs = shaderManager->Compile(L"Resources/Shader/ModelVS.hlsl", ShaderManager::kVertex);
        auto ps = shaderManager->Compile(L"Resources/Shader/ModelPS.hlsl", ShaderManager::kPixel);
        desc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
        desc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());
        desc.BlendState = Helper::BlendDisable;
        desc.DepthStencilState = Helper::DepthStateReadWrite;
        desc.RasterizerState = Helper::RasterizerDefault;
        desc.NumRenderTargets = 1;
        desc.RTVFormats[0] = rtvFormat;
        desc.DSVFormat = dsvFormat;
        desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        desc.SampleDesc.Count = 1;
        pipelineState_->Create(L"Model PSO", desc);
    }
}

void Model::DestroyPipeline() {
    rootSignature_.reset();
    pipelineState_.reset();
}

void Model::Draw(CommandContext& commandContext, const Matrix4x4& world, const Camera& camera, bool isLighting) {
    assert(rootSignature_ && pipelineState_);

    struct TransformationConstantData {
        Matrix4x4 world;
        Matrix4x4 worldViewProj;
    };
    struct MaterialConstantData {
        Vector4 color;
        uint32_t useTexture;
        uint32_t isLighting;
    };

    TransformationConstantData transformationData;
    transformationData.world = world;
    transformationData.worldViewProj = world * camera.GetViewProjectionMatrix();

    commandContext.SetRootSignature(*rootSignature_);
    commandContext.SetPipelineState(*pipelineState_);
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    commandContext.SetDynamicConstantBufferView(0, sizeof(transformationData), &transformationData);
    commandContext.SetVertexBuffer(0, vbView_);
    commandContext.SetIndexBuffer(ibView_);

    switch (samplerType_) {
    default:
    case Model::SamplerType::kLinearWrap:
        commandContext.SetDescriptorTable(3, SamplerManager::LinearWrap);
        break;
    case Model::SamplerType::kLinearClamp:
        commandContext.SetDescriptorTable(3, SamplerManager::LinearClamp);
        break;
    case Model::SamplerType::kPointWrap:
        commandContext.SetDescriptorTable(3, SamplerManager::PointWrap);
        break;
    case Model::SamplerType::kPointClamp:
        commandContext.SetDescriptorTable(3, SamplerManager::PointClamp);
        break;
    }

    for (auto& mesh : meshes_) {
        const auto& material = materials_[mesh.materialIndex];
        MaterialConstantData materialData;
        materialData.color = material.color;
        materialData.useTexture = 0;
        materialData.isLighting = 0;
        if (material.texture) {
            materialData.useTexture = 1;
            commandContext.SetDescriptorTable(2, material.texture->GetSRV());
        }
        if (isLighting) {
            materialData.isLighting = 1;
        }
        commandContext.SetDynamicConstantBufferView(1, sizeof(materialData), &materialData);
        commandContext.DrawIndexed(mesh.indexCount, mesh.indexOffset, mesh.vertexOffset);
    }
}

void Model::LoadMTLFile(const std::filesystem::path& path) {
    std::ifstream file(path);
    assert(file.is_open());

    std::string line;
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream iss(line);
        iss >> identifier;

        // コメントをスキップ
        if (identifier == "#") {
            continue;
        }
        else if (identifier == "newmtl") {
            std::string materialName;
            iss >> materialName;
            materials_.emplace_back();
            materials_.back().name = materialName;
        }
        else if (identifier == "map_Kd") {
            std::string textureName;
            iss >> textureName;
            /*auto p = path.parent_path();
            auto v = path.parent_path() / textureName;
            p;
            v;*/
            materials_.back().texture = std::make_unique<Texture>();
            materials_.back().texture->CreateFromWICFile(path.parent_path() / textureName);
            materials_.back().color = {1.0f,1.0f,1.0f,1.0f};

        }
        else if (identifier == "Kd") {
            Vector4 color;
            iss >> color.x >> color.y >> color.z;
            color.w = 1.0f;
            materials_.back().color = color;
        }
    }

}

void Model::CreateFromObj(const std::filesystem::path& path) {
    std::ifstream file(path);
    assert(file.is_open());

    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    std::unordered_map<std::string, uint16_t> vertexDefinitionMap;

    std::string line;
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream iss(line);
        iss >> identifier;

        if (identifier == "#") {
            continue;
        }
        else if (identifier == "mtllib") {
            std::string materialFileName;
            iss >> materialFileName;
            LoadMTLFile(path.parent_path() / materialFileName);
        }
        else if (identifier == "v") {
            Vector3& position = positions.emplace_back();
            iss >> position.x >> position.y >> position.z;
            // position.z = -position.z;
        }
        else if (identifier == "vn") {
            Vector3& normal = normals.emplace_back();
            iss >> normal.x >> normal.y >> normal.z;
            // normal.z = -normal.z;
        }
        else if (identifier == "vt") {
            Vector2& texcoord = texcoords.emplace_back();
            iss >> texcoord.x >> texcoord.y;
            texcoord.y = 1.0f - texcoord.y;
        }
        else if (identifier == "usemtl") {
            std::string materialName;
            iss >> materialName;
            auto iter = std::find_if(materials_.begin(), materials_.end(),
                [&](const auto& material) {
                    return material.name == materialName;
                });
            if (iter != materials_.end()) {
                Mesh mesh{};
                mesh.indexOffset = uint32_t(indices.size());
                if (!meshes_.empty()) {
                    meshes_.back().indexCount = uint32_t(indices.size()) - meshes_.back().indexOffset;
                }
                mesh.materialIndex = uint32_t(std::distance(materials_.begin(), iter));
                meshes_.emplace_back(mesh);
            }
            else {
                assert(false);
            }
        }
        else if (identifier == "f") {
            // 面の頂点を取得
            std::vector<std::string> vertexDefinitions;
            while (true) {
                std::string vertexDefinition;
                iss >> vertexDefinition;
                if (vertexDefinition.empty()) {
                    break;
                }
                vertexDefinitions.emplace_back(std::move(vertexDefinition));
            }
            assert(vertexDefinitions.size() > 2);
            std::vector<uint16_t> face(vertexDefinitions.size());
            for (uint32_t i = 0; i < vertexDefinitions.size(); ++i) {
                // 頂点が登録済み
                if (vertexDefinitionMap.contains(vertexDefinitions[i])) {
                    face[i] = vertexDefinitionMap[vertexDefinitions[i]];
                }
                else {
                    std::istringstream viss(vertexDefinitions[i]);
                    uint32_t elementIndices[3]{};
                    bool useElement[3]{};
                    for (uint32_t j = 0; j < 3; ++j) {
                        std::string index;
                        std::getline(viss, index, '/');
                        if (!index.empty()) {
                            elementIndices[j] = static_cast<uint32_t>(std::stoi(index)) - 1;
                            useElement[j] = true;
                        }
                    }
                    auto& vertex = vertices.emplace_back();
                    vertex.position = positions[elementIndices[0]];
                    if (useElement[1]) {
                        vertex.texcoord = texcoords[elementIndices[1]];
                    }
                    if (useElement[2]) {
                        vertex.normal = normals[elementIndices[2]];
                    }
                    face[i] = vertexDefinitionMap[vertexDefinitions[i]] = static_cast<uint16_t>(vertices.size() - 1);
                }
            }

            for (uint32_t i = 0; i < face.size() - 2; ++i) {
                indices.emplace_back(face[0]);
                indices.emplace_back(face[i + 1ull]);
                indices.emplace_back(face[i + 2ull]);
            }
        }
    }

    if (!meshes_.empty()) {
        meshes_.back().indexCount = uint32_t(indices.size()) - meshes_.back().indexOffset;
    }

    file.close();

    vertexBuffer_.Create(L"Model VertexBuffer", vertices.size() * sizeof(vertices[0]));
    vertexBuffer_.Copy(vertices.data(), vertices.size() * sizeof(vertices[0]));
    vbView_.BufferLocation = vertexBuffer_.GetGPUVirtualAddress();
    vbView_.SizeInBytes = UINT(vertexBuffer_.GetBufferSize());
    vbView_.StrideInBytes = sizeof(vertices[0]);

    indexBuffer_.Create(L"Model IndexBuffer", indices.size() * sizeof(indices[0]));
    indexBuffer_.Copy(indices.data(), indices.size() * sizeof(indices[0]));
    ibView_.BufferLocation = indexBuffer_.GetGPUVirtualAddress();
    ibView_.SizeInBytes = UINT(indexBuffer_.GetBufferSize());
    ibView_.Format = DXGI_FORMAT_R16_UINT;
}