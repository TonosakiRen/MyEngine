#include "TileBasedRendering.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include <Windows.h>
#include "ShaderManager.h"
#include "Helper.h"
#include "CommandContext.h"
#include "LightNumBuffer.h"
#include "LightManager.h"
#include "Wire.h"
using namespace Microsoft::WRL;
void TileBasedRendering::Initialize()
{

    //compute

    CreatePipeline();

    {
        initialTileFrustrumBuffer_.Create(L"initialTileFrustumBuffer", sizeof(Frustum), kTileNum);
    }

    {
        rwTilesInformation_.Create(L"rwTileInformation", sizeof(ConstBufferData), kTileNum);

    }

    {
        rwPointLightIndex_.Create(L"rwPointLightIndex", sizeof(uint32_t), kTileNum * kMaxInTilePointLight);
    }

    {
        rwSpotLightIndex_.Create(L"rwSpotLightIndex", sizeof(uint32_t), kTileNum * SpotLights::lightNum);
    }

    {
        rwShadowSpotLightIndex_.Create(L"rwShadowSpotLightIndex", sizeof(uint32_t), kTileNum * ShadowSpotLights::lightNum);
    }

}

void TileBasedRendering::ComputeUpdate(CommandContext& commandContext, const ViewProjection& viewProjection)
{
    //初期化
    if (viewProjection_ != &viewProjection) {
        viewProjection_ = &viewProjection;
        initialFrustum_ = viewProjection_->GetFrustum();
        for (int i = 0; i < kTileNum; i++) {
            int height = i / kTileWidthNum;
            int width = i % kTileWidthNum;
            initialTileFrustrum_[i] = GetTileFrustrum(width, height);
        }
        initialTileFrustrumBuffer_.Copy(initialTileFrustrum_, commandContext);
        commandContext.TransitionResource(initialTileFrustrumBuffer_, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    }
    

    LightManager* lightManager = LightManager::GetInstance();

    //dispatch
    commandContext.SetPipelineState(pipelineState_);
    commandContext.SetComputeRootSignature(rootSignature_);

    commandContext.TransitionResource(rwTilesInformation_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    commandContext.TransitionResource(rwPointLightIndex_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    commandContext.TransitionResource(rwSpotLightIndex_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    commandContext.TransitionResource(rwShadowSpotLightIndex_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    commandContext.SetComputeDescriptorTable(UINT(RootParameter::kTileInformation), rwTilesInformation_.GetUAV());
    commandContext.SetComputeDescriptorTable(UINT(RootParameter::kPointLightIndex), rwPointLightIndex_.GetUAV());
    commandContext.SetComputeDescriptorTable(UINT(RootParameter::kSpotLightIndex), rwSpotLightIndex_.GetUAV());
    commandContext.SetComputeDescriptorTable(UINT(RootParameter::kShadowSpotLightIndex), rwShadowSpotLightIndex_.GetUAV());
    commandContext.SetComputeDescriptorTable(UINT(RootParameter::kInitialTileFrustum), initialTileFrustrumBuffer_.GetSRV());
    commandContext.SetComputeDescriptorTable(UINT(RootParameter::kPointLights), lightManager->pointLights_->structureBuffer_.GetSRV());
    commandContext.SetComputeDescriptorTable(UINT(RootParameter::kSpotLights), lightManager->spotLights_->structureBuffer_.GetSRV());
    commandContext.SetComputeDescriptorTable(UINT(RootParameter::kShadowSpotLights), lightManager->shadowSpotLights_->structureBuffer_.GetSRV());
    commandContext.SetComputeConstantBuffer(UINT(RootParameter::kLightNum), lightManager->lightNumBuffer_->GetGPUVirtualAddress());
    commandContext.SetComputeConstantBuffer(UINT(RootParameter::kViewProjection), viewProjection.GetGPUVirtualAddress());

    commandContext.Dispatch(1, 1, 1);
    commandContext.UAVBarrier(rwTilesInformation_);
    commandContext.UAVBarrier(rwPointLightIndex_);
    commandContext.UAVBarrier(rwSpotLightIndex_);
    commandContext.UAVBarrier(rwShadowSpotLightIndex_);
}

void TileBasedRendering::CreatePipeline()
{
    ComPtr<IDxcBlob> uavBlob;

    auto shaderManager = ShaderManager::GetInstance();
    uavBlob = shaderManager->Compile(L"TBRCS.hlsl", ShaderManager::kCompute);
    assert(uavBlob != nullptr);

    CD3DX12_DESCRIPTOR_RANGE ranges[8]{};
    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, int(RootParameter::kTileInformation));
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, int(RootParameter::kPointLightIndex));
    ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, int(RootParameter::kSpotLightIndex));
    ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, int(RootParameter::kShadowSpotLightIndex));

    ranges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    ranges[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
    ranges[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
    ranges[7].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);


    CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::ParameterNum)]{};
    rootparams[int(RootParameter::kTileInformation)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kTileInformation)]);
    rootparams[int(RootParameter::kPointLightIndex)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kPointLightIndex)]);
    rootparams[int(RootParameter::kSpotLightIndex)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kSpotLightIndex)]);
    rootparams[int(RootParameter::kShadowSpotLightIndex)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kShadowSpotLightIndex)]);
    rootparams[int(RootParameter::kInitialTileFrustum)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kInitialTileFrustum)]);
    rootparams[int(RootParameter::kPointLights)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kPointLights)]);
    rootparams[int(RootParameter::kSpotLights)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kSpotLights)]);
    rootparams[int(RootParameter::kShadowSpotLights)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kShadowSpotLights)]);
    rootparams[int(RootParameter::kLightNum)].InitAsConstantBufferView(0, 0);
    rootparams[int(RootParameter::kViewProjection)].InitAsConstantBufferView(1, 0);

    // ルートシグネチャの設定
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.pParameters = rootparams;
    rootSignatureDesc.NumParameters = _countof(rootparams);

    rootSignature_.Create(L"TBRRootSignature",rootSignatureDesc);

    D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
    desc.pRootSignature = rootSignature_;
    desc.CS = CD3DX12_SHADER_BYTECODE(uavBlob->GetBufferPointer(), uavBlob->GetBufferSize());

    pipelineState_.Create(L"TBRPipeline", desc);
}

Frustum TileBasedRendering::GetTileFrustrum(const int& width, const int& height)
{
    Frustum result;
    Vector3 vertex[8];

    float wNum = float(width);
    float hNum = float(height);

    Vector3 frontWidthVec = initialFrustum_.vertex[1] - initialFrustum_.vertex[0];
    Vector3 frontHeightVec = initialFrustum_.vertex[2] - initialFrustum_.vertex[0];

    Vector3 backWidthVec = initialFrustum_.vertex[5] - initialFrustum_.vertex[4];
    Vector3 backHeightVec = initialFrustum_.vertex[6] - initialFrustum_.vertex[4];

    frontWidthVec = frontWidthVec / kTileWidthNum;
    frontHeightVec = frontHeightVec / kTileHeightNum;

    backWidthVec = backWidthVec / kTileWidthNum;
    backHeightVec = backHeightVec / kTileHeightNum;

    Vector3 tileWidthLeft = frontWidthVec * wNum;
    Vector3 tileWidthRight = frontWidthVec * (wNum + 1);
    Vector3 tileHeightTop = frontHeightVec * hNum;
    Vector3 tileHeightBottom = frontHeightVec * (hNum + 1);

    vertex[0] = initialFrustum_.vertex[0] + tileWidthLeft + tileHeightTop;
    vertex[1] = initialFrustum_.vertex[0] + tileWidthRight + tileHeightTop;
    vertex[2] = initialFrustum_.vertex[0] + tileWidthLeft + tileHeightBottom;
    vertex[3] = initialFrustum_.vertex[0] + tileWidthRight + tileHeightBottom;

    tileWidthLeft = backWidthVec * wNum;
    tileWidthRight = backWidthVec * (wNum + 1);
    tileHeightTop = backHeightVec * hNum;
    tileHeightBottom = backHeightVec * (hNum + 1);

    vertex[4] = initialFrustum_.vertex[4] + tileWidthLeft + tileHeightTop;
    vertex[5] = initialFrustum_.vertex[4] + tileWidthRight + tileHeightTop;
    vertex[6] = initialFrustum_.vertex[4] + tileWidthLeft + tileHeightBottom;
    vertex[7] = initialFrustum_.vertex[4] + tileWidthRight + tileHeightBottom;

    result = MakeFrustum(vertex[0], vertex[1], vertex[2], vertex[3], vertex[4], vertex[5], vertex[6], vertex[7]);

    return result;
}