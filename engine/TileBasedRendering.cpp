#include "TileBasedRendering.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include <Windows.h>
#include "ShaderManager.h"
#include "Helper.h"
#include "CommandContext.h"
#include "LightNumBuffer.h"
using namespace Microsoft::WRL;
void TileBasedRendering::Initialize()
{
    {
        tileInformationBuffer_.Create(sizeof(ConstBufferData), kTileNum);
    }

    {
        pointLightIndexBuffer_.Create(sizeof(uint32_t), kTileNum * PointLights::lightNum);
    }

    {
        spotLightIndexBuffer_.Create(sizeof(uint32_t), kTileNum * SpotLights::lightNum);
    }

    {
        shadowSpotLightIndexBuffer_.Create(sizeof(uint32_t), kTileNum * ShadowSpotLights::lightNum);
    }

    //compute

    CreatePipeline();

    {
        initialTileFrustrumBuffer_.Create(sizeof(Frustum), kTileNum);
    }

    {
        rwTilesInformation_.Create(sizeof(ConstBufferData), kTileNum);
 
    }

    {
        rwPointLightIndex_.Create(sizeof(uint32_t), kTileNum * PointLights::lightNum);
    }

    {
        rwSpotLightIndex_.Create(sizeof(uint32_t), kTileNum * SpotLights::lightNum);
    }

    {
        rwShadowSpotLightIndex_.Create(sizeof(uint32_t), kTileNum * ShadowSpotLights::lightNum);
    }

}

void TileBasedRendering::Update(const ViewProjection& viewProjection, const PointLights& pointLights,const SpotLights& spotLights, const ShadowSpotLights& shadowSpotLights)
{

    //初期化
    if (viewProjection_ != &viewProjection) {
        viewProjection_ = &viewProjection;
        Matrix4x4 m = MakeAffineMatrix(Vector3{ 1.0f,1.0f,1.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });
        Matrix4x4 in = Inverse(m);
        initialFrustum_ = MakeFrustrum(Inverse(in * viewProjection_->GetMatProjection()));
        for (int i = 0; i < kTileNum; i++) {
            int height = i / kTileWidthNum;
            int width = i % kTileWidthNum;
            initialTileFrustrum_[i] = GetTileFrustrum(width, height);
        }

    }
    
    for (int i = 0; i < kTileNum; i++) {
        tilesInformation_[i].pointLightNum = 0;
        tilesInformation_[i].spotLightNum = 0;
        tilesInformation_[i].shadowSpotLightNum = 0;
        tilesInformation_[i].pointLightIndex.clear();
        tilesInformation_[i].spotLightIndex.clear();
        tilesInformation_[i].shadowSpotLightIndex.clear();

        tileFrustrum_[i] = initialTileFrustrum_[i] * viewProjection_->GetWorldMatrix();
    }

    for (uint32_t i = 0; i < PointLights::lightNum;i++) {
        if (pointLights.lights_[i].isActive) {
            for (uint32_t j = 0; j < kTileNum; j++) {
                //pointLights.lights_[i];
                bool isHit = IsHitSphere(tileFrustrum_[j], MakeTranslation(pointLights.lights_[i].worldTransform.matWorld_), pointLights.lights_[i].radius);
                //当たった
                if (isHit) {
                    tilesInformation_[j].pointLightNum++;
                    tilesInformation_[j].pointLightIndex.push_back(i);
                }
            }
        }
    }

    for (uint32_t i = 0; i < SpotLights::lightNum; i++) {
        if (spotLights.lights_[i].isActive) {
            for (uint32_t j = 0; j < kTileNum; j++) {
                //tilesInformation_[j];
                //spotLights.lights_[i];
                //当たった
                if (true) {
                    tilesInformation_[j].spotLightNum++;
                    tilesInformation_[j].spotLightIndex.push_back(i);
                }
            }
        }
    }

    for (uint32_t i = 0; i < ShadowSpotLights::lightNum; i++) {
        if (shadowSpotLights.lights_[i].isActive) {
            for (uint32_t j = 0; j < kTileNum; j++) {
                //tilesInformation_[j];
                //shadowSpotLights.lights_[i];
                //当たった
                if (true) {
                    tilesInformation_[j].shadowSpotLightNum++;
                    tilesInformation_[j].shadowSpotLightIndex.push_back(i);
                }
            }
        }
    }

    //コピー
    std::vector<ConstBufferData> bufferData;
    bufferData.reserve(kTileNum);

    uint32_t pointLightIndex[kTileNum * PointLights::lightNum ];
    uint32_t spotLightLightIndex[kTileNum * SpotLights::lightNum ];
    uint32_t shadowSpotLightLightIndex[kTileNum * ShadowSpotLights::lightNum];

    for (uint32_t i = 0; i < kTileNum; i++) {
        ConstBufferData data;
      
        data.pointLightNum = tilesInformation_[i].pointLightNum;
        data.spotLightNum = tilesInformation_[i].spotLightNum;
        data.shadowSpotLightNum = tilesInformation_[i].shadowSpotLightNum;

        for (int j = 0; uint32_t index : tilesInformation_[i].pointLightIndex) {
            pointLightIndex[i * PointLights::lightNum + j] = index;
            j++;
        }
        for (int j = 0; uint32_t index : tilesInformation_[i].spotLightIndex) {
            spotLightLightIndex[i * SpotLights::lightNum + j] = index;
            j++;
        }
        for (int j = 0; uint32_t index : tilesInformation_[i].shadowSpotLightIndex) {
            shadowSpotLightLightIndex[i * ShadowSpotLights::lightNum + j] = index;
            j++;
        }
        bufferData.emplace_back(data);
    }

    tileInformationBuffer_.Copy(bufferData.data(), sizeof(bufferData[0]) * bufferData.size());
    pointLightIndexBuffer_.Copy(pointLightIndex);
    spotLightIndexBuffer_.Copy(spotLightLightIndex);
    shadowSpotLightIndexBuffer_.Copy(shadowSpotLightLightIndex);
}

void TileBasedRendering::ComputeUpdate(CommandContext& commandContext, const ViewProjection& viewProjection, PointLights& pointLights, const SpotLights& spotLights, const ShadowSpotLights& shadowSpotLights, LightNumBuffer& lightNumBuffer)
{
    //初期化
    if (viewProjection_ != &viewProjection) {
        viewProjection_ = &viewProjection;
        Matrix4x4 m = MakeAffineMatrix(Vector3{ 1.0f,1.0f,1.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });
        Matrix4x4 in = Inverse(m);
        initialFrustum_ = MakeFrustrum(Inverse(in * viewProjection_->GetMatProjection()));
        for (int i = 0; i < kTileNum; i++) {
            int height = i / kTileWidthNum;
            int width = i % kTileWidthNum;
            initialTileFrustrum_[i] = GetTileFrustrum(width, height);
            initialTileFrustrumBuffer_.Copy(initialTileFrustrum_);
        }
    }


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
    commandContext.SetComputeDescriptorTable(UINT(RootParameter::kInitialTileFrustum), initialTileFrustrumBuffer_.GetSRV(commandContext));
    commandContext.SetComputeDescriptorTable(UINT(RootParameter::kPointLights), pointLights.structureBuffer_.GetSRV());
    commandContext.SetComputeConstantBuffer(UINT(RootParameter::kLightNum),lightNumBuffer.GetGPUVirtualAddress());
    commandContext.SetComputeConstantBuffer(UINT(RootParameter::kViewProjection), viewProjection.GetGPUVirtualAddress());

    commandContext.Dispatch(1,1,1);
}

void TileBasedRendering::CreatePipeline()
{
    ComPtr<IDxcBlob> uavBlob;

    auto shaderManager = ShaderManager::GetInstance();
    uavBlob = shaderManager->Compile(L"TBR.hlsl", ShaderManager::kCompute);
    assert(uavBlob != nullptr);

    CD3DX12_DESCRIPTOR_RANGE ranges[6]{};
    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, int(RootParameter::kTileInformation));
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, int(RootParameter::kPointLightIndex));
    ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, int(RootParameter::kSpotLightIndex));
    ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, int(RootParameter::kShadowSpotLightIndex));

    ranges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    ranges[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);


    CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::ParameterNum)]{};
    rootparams[int(RootParameter::kTileInformation)].InitAsDescriptorTable(1,&ranges[int(RootParameter::kTileInformation)]);
    rootparams[int(RootParameter::kPointLightIndex)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kPointLightIndex)]);
    rootparams[int(RootParameter::kSpotLightIndex)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kSpotLightIndex)]);
    rootparams[int(RootParameter::kShadowSpotLightIndex)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kShadowSpotLightIndex)]);
    rootparams[int(RootParameter::kInitialTileFrustum)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kInitialTileFrustum)]);
    rootparams[int(RootParameter::kPointLights)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kPointLights)]);
    rootparams[int(RootParameter::kLightNum)].InitAsConstantBufferView(0,0);
    rootparams[int(RootParameter::kViewProjection)].InitAsConstantBufferView(1, 0);

    // ルートシグネチャの設定
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.pParameters = rootparams;
    rootSignatureDesc.NumParameters = _countof(rootparams);

    rootSignature_.Create(rootSignatureDesc);

    D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
    desc.pRootSignature = rootSignature_;
    desc.CS = CD3DX12_SHADER_BYTECODE(uavBlob->GetBufferPointer(), uavBlob->GetBufferSize());

    pipelineState_.Create(desc);
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

bool TileBasedRendering::IsHitSphere(const Frustum& frustum, const Vector3& position, const float radius)
{
    int hitNum = 0;
    //平面の法線と内積をとる
    for (int i = 0; i < 6; i++) {
        //プラスであれば外側距離を測る,内側の場合マイナス
        float a = Dot(frustum.plane[i].normal, position) - frustum.plane[i].distance;
        if (a < 0.0f) {
            hitNum++;
        }
        else {
            if (std::abs(a) < radius) {
                hitNum++;
            }
        }
    }

    if (hitNum == 6) {
        return true;
    }

    return false;
}
