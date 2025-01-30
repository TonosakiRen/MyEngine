#include "Animation/Skinning.h"
/**
 * @file Skinning.cpp
 * @brief Skinningをするためのpipeline
 */
#include <array>

#include "Texture/TextureManager.h"
#include "Model/ModelManager.h"
#include "Light/ShaderManager.h"
#include "ImGuiManager.h"
#include "Render/Renderer.h"
#include "Model/Model.h"


using namespace Microsoft::WRL;

void Skinning::Initialize() {
    CreatePipeline();
}

void Skinning::Finalize()
{
    rootSignature_.reset();
    pipelineState_.reset();
}

void Skinning::PreDispatch(CommandContext& commandContext)
{
    commandContext.SetPipelineState(*pipelineState_);
    commandContext.SetComputeRootSignature(*rootSignature_);
}

void Skinning::CreatePipeline()
{
    ComPtr<IDxcBlob> uavBlob;

    auto shaderManager = ShaderManager::GetInstance();
    uavBlob = shaderManager->Compile(L"SkinningCS.hlsl", ShaderManager::kCompute);
    assert(uavBlob != nullptr);


    rootSignature_ = std::make_unique<RootSignature>();
    pipelineState_ = std::make_unique<PipelineState>();

    CD3DX12_DESCRIPTOR_RANGE ranges[4]{};
    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, int(RootParameter::kMatrixPalette));
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, int(RootParameter::kInputVertices));
    ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, int(RootParameter::kInfluences));

    ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

    CD3DX12_ROOT_PARAMETER rootparams[5]{};
    rootparams[int(RootParameter::kMatrixPalette)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kMatrixPalette)]);
    rootparams[int(RootParameter::kInputVertices)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kInputVertices)]);
    rootparams[int(RootParameter::kInfluences)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kInfluences)]);
    rootparams[int(RootParameter::kSkinnedVertices)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kSkinnedVertices)]);
    rootparams[int(RootParameter::kSkinningInformation)].InitAsConstantBufferView(0,0);

    // ルートシグネチャの設定
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.pParameters = rootparams;
    rootSignatureDesc.NumParameters = _countof(rootparams);

    rootSignature_->Create(L"skinningRootSignature", rootSignatureDesc);

    D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
    desc.pRootSignature = *rootSignature_;
    desc.CS = CD3DX12_SHADER_BYTECODE(uavBlob->GetBufferPointer(), uavBlob->GetBufferSize());

    pipelineState_->Create(L"skinningPipeline", desc);
}

void Skinning::Dispatch(CommandContext& commandContext, const uint32_t modelHandle,SkinCluster& skinCluster)
{

    Engine::ModelManager* modelManager = Engine::ModelManager::GetInstance();
    commandContext.TransitionResource(skinCluster.GetSkinnedVertices(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    commandContext.SetComputeDescriptorTable(static_cast<UINT>(RootParameter::kMatrixPalette), skinCluster.GetPaletteResourceSRV());
    commandContext.SetComputeDescriptorTable(static_cast<UINT>(RootParameter::kInputVertices), modelManager->GetModelData(modelHandle).meshes[0].GetVerticesSRV());
    commandContext.SetComputeDescriptorTable(static_cast<UINT>(RootParameter::kInfluences), skinCluster.GetInfluencesSRV());
    commandContext.SetComputeDescriptorTable(static_cast<UINT>(RootParameter::kSkinnedVertices), skinCluster.GetSkinnedVerticesUAV());
    commandContext.SetComputeConstantBuffer(static_cast<UINT>(RootParameter::kSkinningInformation), skinCluster.GetSkinningInformationGPUAddress());
    Mesh& mesh = modelManager->GetModelData(modelHandle).meshes[0];
    commandContext.TransitionResource(mesh.vertexBuffer_, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    commandContext.TransitionResource(mesh.indexBuffer_, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    commandContext.Dispatch(UINT(modelManager->GetModelData(modelHandle).meshes[0].GetVerticies().size() + 1023) / 1024,1,1);
}
