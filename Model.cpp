#include "Model.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <d3dcompiler.h>
#include <fstream>
#include <sstream>
#pragma comment(lib, "d3dcompiler.lib")
#include "TextureManager.h"
#include "ShaderManager.h"
#include "Renderer.h"

using namespace DirectX;
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* Model::commandList_ = nullptr;
std::unique_ptr<RootSignature> Model::rootSignature_;
std::unique_ptr<PipelineState> Model::pipelineState_;
void Model::StaticInitialize() {
    CreatePipeline();
}

void Model::PreDraw(ID3D12GraphicsCommandList* commandList) {
    assert(Model::commandList_ == nullptr);

    commandList_ = commandList;

    commandList->SetPipelineState(*pipelineState_);
    commandList->SetGraphicsRootSignature(*rootSignature_);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::PostDraw() {
    commandList_ = nullptr;
}

Model* Model::Create() {
    Model* object3d = new Model();
    assert(object3d);

    // 初期化
    object3d->Initialize();

    return object3d;
}

Model* Model::Create(std::string name) {
    Model* object3d = new Model();
    assert(object3d);

    object3d->Initialize(name);

    return object3d;
}

void Model::CreatePipeline() {
    HRESULT result = S_FALSE;
    ComPtr<IDxcBlob> vsBlob;    
    ComPtr<IDxcBlob> psBlob;    
    ComPtr<ID3DBlob> errorBlob; 

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"BasicVS.hlsl",ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    psBlob = shaderManager->Compile(L"BasicPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    rootSignature_ = std::make_unique<RootSignature>();
    pipelineState_ = std::make_unique<PipelineState>();

    {

        // デスクリプタレンジ
        CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
        descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[5] = {};
        rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[4].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);

        // スタティックサンプラー
        CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
            CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

        // ルートシグネチャの設定
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.pParameters = rootparams;
        rootSignatureDesc.NumParameters = _countof(rootparams);
        rootSignatureDesc.pStaticSamplers = &samplerDesc;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        rootSignature_->Create(rootSignatureDesc);

    }

    {


        D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
          {
           "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
          {
           "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
          {
           "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT,
           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        };

        // グラフィックスパイプラインの流れを設定
        D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
        gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
        gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());

        // サンプルマスク
        gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
        // ラスタライザステート
        gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        //  デプスステンシルステート
        gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

        // レンダーターゲットのブレンド設定
        D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
        blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        blenddesc.BlendEnable = true;
        blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
        blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

        blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
        blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

        // ブレンドステートの設定
        gpipeline.BlendState.RenderTarget[0] = blenddesc;

        // 深度バッファのフォーマット
        gpipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();

        // 頂点レイアウトの設定
        gpipeline.InputLayout.pInputElementDescs = inputLayout;
        gpipeline.InputLayout.NumElements = _countof(inputLayout);

        // 図形の形状設定（三角形）
        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        gpipeline.NumRenderTargets = Renderer::kRenderTargetNum;
        gpipeline.RTVFormats[int(Renderer::kMain)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kMain);
        gpipeline.RTVFormats[int(Renderer::kNormal)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kNormal);// 0～255指定のRGBA
        gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

        gpipeline.pRootSignature = *rootSignature_;

        // グラフィックスパイプラインの生成
        pipelineState_->Create(gpipeline);
    }
}

void Model::CreateMesh() {
    HRESULT result = S_FALSE;
    if (isModelLoad_) {
        std::string line;//ファイルから読んだ1行を格納するもの
        std::vector<Vector3> positions;//位置
        std::vector<Vector3> normals;//法線
        std::vector<Vector2> texcoords;//テクスチャ座標
        std::string directoryPath = "Resources/models/" + name_ + "/";
        std::ifstream file( directoryPath + name_ +".obj"); //ファイルを開く
        uvHandle_ = 0; //とりあえず1x1White
        assert(file.is_open());//とりあえず開けなったら止める

        while (std::getline(file, line)) {
            std::string identifier;
            std::istringstream s(line);
            s >> identifier;//先頭の識別子を読む

            if (identifier == "v") {
                Vector3 position;
                s >> position.x >> position.y >> position.z;
                position.z *= -1.0f;
                /*position.w = 1.0f;*/
                positions.push_back(position);
            }
            else if (identifier == "vt") {
                Vector2 texcoord;
                s >> texcoord.x >> texcoord.y;
                texcoord.y = 1.0f - texcoord.y;
                texcoords.push_back(texcoord);
            }
            else if (identifier == "vn") {
                Vector3 normal;
                s >> normal.x >> normal.y >> normal.z;
                normal.z *= -1.0f;
                normals.push_back(normal);
            }
            else if (identifier == "f") {
                VertexData triangle[3];
                //面は三角形限定。その他は未対応
                for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
                    std::string vertexDefinition;
                    s >> vertexDefinition;
                    //頂点の要素へのIndexは[位置/UV/法線]で格納されているので、分解してIndexを取得する
                    std::istringstream v(vertexDefinition);
                    uint32_t elementIndices[3];
                    for (int32_t element = 0; element < 3; ++element) {
                        std::string index;
                        std::getline(v, index, '/');//区切りでインデックスを読んでいく
                        elementIndices[element] = std::stoi(index);
                    }
                    //要素へのIndexから、実際の要素の要素の値
                    Vector3 position = positions[elementIndices[0] - 1];
                    Vector2 texcoord = texcoords[elementIndices[1] - 1];
                    Vector3 normal = normals[elementIndices[2] - 1];

                    //頂点を逆順で登録することで、回り順を逆にする
                    triangle[faceVertex] = { position,normal,texcoord };

                }
                //頂点を逆順で登録することで、周り順を逆にする
                vertices_.push_back(triangle[2]);
                vertices_.push_back(triangle[1]);
                vertices_.push_back(triangle[0]);
            }
            else if (identifier == "mtllib") {
                //materialTemplateLibraryファイルの名前を取得する
                std::string materialFilename;
                s >> materialFilename;
                //基本的にobjファイルと同一階層にmtlは存在させるので,ディレクトリ名とファイル名を渡す

                std::string uvFilePass;//構築するMaterialData
                std::string line;//fileから読んだ１行を格納するもの
                std::ifstream file("Resources/models/" + name_ + "/" + name_ + ".mtl"); //ファイルを開く
                assert(file.is_open());//とりあえず開けなかったら止める
                while (std::getline(file, line)) {
                    std::string identifier;
                    std::istringstream s(line);
                    s >> identifier;
                    //identifierに応じた処理
                    if (identifier == "map_Kd") {
                        std::string textureFilename;
                        s >> textureFilename;
                        //連結してファイルパスにする
                        uvFilePass = directoryPath + textureFilename;
                        uvHandle_ = TextureManager::LoadUv(textureFilename, uvFilePass);
                    }
                }
            }
        }
    }
    else {
        vertices_ = {
            //  x      y      z       nx     ny    nz       u     v
            // 前
              {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // 左下
              {{-1.0f, +1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 左上
              {{+1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // 右下
              {{+1.0f, +1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, // 右上
              // 後(前面とZ座標の符号が逆)
              {{+1.0f, -1.0f, +1.0f}, {0.0f, 0.0f, +1.0f}, {0.0f, 1.0f}}, // 左下
              {{+1.0f, +1.0f, +1.0f}, {0.0f, 0.0f, +1.0f}, {0.0f, 0.0f}}, // 左上
              {{-1.0f, -1.0f, +1.0f}, {0.0f, 0.0f, +1.0f}, {1.0f, 1.0f}}, // 右下
              {{-1.0f, +1.0f, +1.0f}, {0.0f, 0.0f, +1.0f}, {1.0f, 0.0f}}, // 右上
              // 左
              {{-1.0f, -1.0f, +1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 左下
              {{-1.0f, +1.0f, +1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 左上
              {{-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 右下
              {{-1.0f, +1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 右上
              // 右（左面とX座標の符号が逆）
              {{+1.0f, -1.0f, -1.0f}, {+1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 左下
              {{+1.0f, +1.0f, -1.0f}, {+1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 左上
              {{+1.0f, -1.0f, +1.0f}, {+1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 右下
              {{+1.0f, +1.0f, +1.0f}, {+1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 右上
              // 下
              {{+1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // 左下
              {{+1.0f, -1.0f, +1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // 左上
              {{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // 右下
              {{-1.0f, -1.0f, +1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // 右上
              // 上（下面とY座標の符号が逆）
              {{-1.0f, +1.0f, -1.0f}, {0.0f, +1.0f, 0.0f}, {0.0f, 1.0f}}, // 左下
              {{-1.0f, +1.0f, +1.0f}, {0.0f, +1.0f, 0.0f}, {0.0f, 0.0f}}, // 左上
              {{+1.0f, +1.0f, -1.0f}, {0.0f, +1.0f, 0.0f}, {1.0f, 1.0f}}, // 右下
              {{+1.0f, +1.0f, +1.0f}, {0.0f, +1.0f, 0.0f}, {1.0f, 0.0f}}, // 右上
        };

        // 頂点インデックスの設定
        indices_ = { 0,  1,  3,  3,  2,  0,

                    4,  5,  7,  7,  6,  4,

                    8,  9,  11, 11, 10, 8,

                    12, 13, 15, 15, 14, 12,

                    16, 17, 19, 19, 18, 16,

                    20, 21, 23, 23, 22, 20 };

    }
    
    // 頂点データのサイズ
    UINT sizeVB = static_cast<UINT>(sizeof(VertexData) * vertices_.size());

    vertexBuffer_.Create(sizeVB);

    vertexBuffer_.Copy(vertices_.data(), sizeVB);

    // 頂点バッファビューの作成
    vbView_.BufferLocation = vertexBuffer_.GetGPUVirtualAddress();
    vbView_.SizeInBytes = sizeVB;
    vbView_.StrideInBytes = sizeof(vertices_[0]);

    if (isModelLoad_ == false) {
        // インデックスデータのサイズ
        UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices_.size());

        indexBuffer_.Create(sizeIB);

        indexBuffer_.Copy(indices_.data(), sizeIB);

        // インデックスバッファビューの作成
        ibView_.BufferLocation = indexBuffer_.GetGPUVirtualAddress();
        ibView_.Format = DXGI_FORMAT_R16_UINT;
        ibView_.SizeInBytes = sizeIB;
    }

}

void Model::Initialize() {
    // メッシュ生成
    CreateMesh();
}

void Model::Initialize(std::string name) {
    isModelLoad_ = true;
    name_ = name;

    // メッシュ生成
    CreateMesh();
}

void Model::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection,const DirectionalLight& directionalLight, const Material& material,uint32_t textureHadle) {

    // 頂点バッファの設定
    commandList_->IASetVertexBuffers(0, 1, &vbView_);

    // インデックスバッファの設定
    commandList_->IASetIndexBuffer(&ibView_);

    // CBVをセット（ワールド行列）
    commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kWorldTransform),worldTransform.GetGPUVirtualAddress());

    // CBVをセット（ビュープロジェクション行列）
    commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kViewProjection),viewProjection.GetGPUVirtualAddress());

    // CBVをセット（ライト）
    commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kDirectionalLight), directionalLight.GetGPUVirtualAddress());

    // CBVをセット（マテリアル）
    commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kMaterial), material.GetGPUVirtualAddress());

    // SRVをセット
    TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, static_cast<UINT>(RootParameter::kTexture), textureHadle);

    // 描画コマンド
    commandList_->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
}

void Model::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, const DirectionalLight& directionalLight, const Material& material) {
    assert(commandList_);

    // 頂点バッファの設定
    commandList_->IASetVertexBuffers(0, 1, &vbView_);

    // CBVをセット（ワールド行列）
    commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

    // CBVをセット（ビュープロジェクション行列）
    commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kViewProjection), viewProjection.GetGPUVirtualAddress());

    // CBVをセット（ライト）
    commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kDirectionalLight), directionalLight.GetGPUVirtualAddress());

    // CBVをセット（マテリアル）
    commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kMaterial), material.GetGPUVirtualAddress());

    // SRVをセット
    TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, static_cast<UINT>(RootParameter::kTexture), uvHandle_);

    // 描画コマンド
    commandList_->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);
}
