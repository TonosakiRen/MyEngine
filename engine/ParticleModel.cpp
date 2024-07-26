#include "ParticleModel.h"
#include "ShaderManager.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "LightManager.h"

using namespace Microsoft::WRL;

void ParticleModel::Initialize() {
    switch (Renderer::GetRenderingMode())
    {
    case Renderer::kForward:
        CreateForwardPipeline();
        break;
    case Renderer::kDeferred:
        CreatePipeline();
        break;
    default:
        break;
    }
}

void ParticleModel::Finalize()
{
    for (int i = 0; i < kPipelineNum; i++) {
        rootSignature_[i].reset();
        pipelineState_[i].reset();
    }
}

void ParticleModel::PreDraw(PipelineType pipelineType,CommandContext& commandContext, const ViewProjection& viewProjection, const TileBasedRendering& tileBasedRendering) {
    
    commandContext.SetPipelineState(*pipelineState_[pipelineType]);
    commandContext.SetGraphicsRootSignature(*rootSignature_[pipelineType]);
    LightManager* lightManager = LightManager::GetInstance();

    switch (Renderer::GetRenderingMode())
    {
    case Renderer::kForward:

        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kDirectionalLights), lightManager->directionalLights_->srvHandle_);
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kPointLights), lightManager->pointLights_->structureBuffer_.GetSRV());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kAreaLights), lightManager->areaLights_->structureBuffer_.GetSRV());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kSpotLights), lightManager->spotLights_->srvHandle_);
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kShadowSpotLights), lightManager->shadowSpotLights_->srvHandle_);
        commandContext.SetConstantBuffer(static_cast<UINT>(ForwardRootParameter::kViewProjection), viewProjection.GetGPUVirtualAddress());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kTileInformation), tileBasedRendering.GetTileInformationGPUHandle());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kTBRPointLightIndex), tileBasedRendering.GetPointLightIndexGPUHandle());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kTBRSpotLightIndex), tileBasedRendering.GetSpotLightIndexGPUHandle());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kTBRShadowSpotLightIndex), tileBasedRendering.GetShadowSpotLightIndexGPUHandle());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::k2DTextures), DirectXCommon::GetInstance()->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).GetDiscriptorStartHandle());
        commandContext.SetConstants(static_cast<UINT>(ForwardRootParameter::kTileNum), TileBasedRendering::kTileWidthNum, TileBasedRendering::kTileHeightNum);
        break;
    case Renderer::kDeferred:
        commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kViewProjection), viewProjection.GetGPUVirtualAddress());
        break;
    default:
        break;
    }
  
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void ParticleModel::CreatePipeline() {
    ComPtr<IDxcBlob> vsBlob;
    ComPtr<IDxcBlob> psBlob;

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"ParticleModelVS.hlsl", ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    psBlob = shaderManager->Compile(L"ParticleModelPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    rootSignature_[kDeferred] = std::make_unique<RootSignature>();
    pipelineState_[kDeferred] = std::make_unique<PipelineState>();

    {

        CD3DX12_DESCRIPTOR_RANGE descRangeSRV[int(RootParameter::kDescriptorRangeNum)];
        descRangeSRV[int(RootParameter::kTexture)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0
        descRangeSRV[int(RootParameter::kParticleData)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1


        CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::parameterNum)] = {};
        rootparams[int(RootParameter::kTexture)].InitAsDescriptorTable(1, &descRangeSRV[int(RootParameter::kTexture)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kParticleData)].InitAsDescriptorTable(1, &descRangeSRV[int(RootParameter::kParticleData)], D3D12_SHADER_VISIBILITY_ALL);

        rootparams[int(RootParameter::kViewProjection)].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kMaterial)].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

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

        rootSignature_[kDeferred]->Create(L"particleModelRootSignature", rootSignatureDesc);

    }

    {

        // 頂点レイアウト
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

        // グラフィックスパイプライン
        D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
        gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
        gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());


        gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

        gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

        gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);


        D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
        blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        blenddesc.BlendEnable = true;
        blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
        blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

        blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
        blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

        // レンダーターゲットのブレンド設定
        D3D12_RENDER_TARGET_BLEND_DESC blenddesc2{};
        blenddesc2.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        blenddesc2.BlendEnable = false;
        blenddesc2.BlendOp = D3D12_BLEND_OP_ADD;
        blenddesc2.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blenddesc2.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

        blenddesc2.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        blenddesc2.SrcBlendAlpha = D3D12_BLEND_ONE;
        blenddesc2.DestBlendAlpha = D3D12_BLEND_ZERO;

        // ブレンドステートの設定
        gpipeline.BlendState.IndependentBlendEnable = true;
        gpipeline.BlendState.RenderTarget[0] = blenddesc;
        gpipeline.BlendState.RenderTarget[1] = blenddesc;
        gpipeline.BlendState.RenderTarget[2] = blenddesc2;

        gpipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();

        gpipeline.InputLayout.pInputElementDescs = inputLayout;
        gpipeline.InputLayout.NumElements = _countof(inputLayout);

        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        gpipeline.NumRenderTargets = Renderer::kRenderTargetNum;
        gpipeline.RTVFormats[int(Renderer::kColor)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor);
        gpipeline.RTVFormats[int(Renderer::kNormal)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kNormal);
        gpipeline.RTVFormats[int(Renderer::kMaterial)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kMaterial);
        gpipeline.SampleDesc.Count = 1;

        gpipeline.pRootSignature = *rootSignature_[kDeferred];

        // グラフィックスパイプラインの生成
        pipelineState_[kDeferred]->Create(L"particleModelPipeline", gpipeline);
    }

}

void ParticleModel::CreateForwardPipeline()
{
    ComPtr<IDxcBlob> vsBlob;
    ComPtr<IDxcBlob> psBlob;

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"ParticleModelVS.hlsl", ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    psBlob = shaderManager->Compile(L"forwardPlus/FParticleModelPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    rootSignature_[kForward] = std::make_unique<RootSignature>();
    pipelineState_[kForward] = std::make_unique<PipelineState>();

    {

        CD3DX12_DESCRIPTOR_RANGE descRangeSRV[int(ForwardRootParameter::kDescriptorRangeNum)];
        descRangeSRV[int(ForwardRootParameter::kTexture)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0
        descRangeSRV[int(ForwardRootParameter::kParticleData)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1
        descRangeSRV[int(ForwardRootParameter::kDirectionalLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
        descRangeSRV[int(ForwardRootParameter::kPointLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
        descRangeSRV[int(ForwardRootParameter::kAreaLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);
        descRangeSRV[int(ForwardRootParameter::kSpotLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);
        descRangeSRV[int(ForwardRootParameter::kShadowSpotLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6);
        descRangeSRV[int(ForwardRootParameter::kTileInformation)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
        descRangeSRV[int(ForwardRootParameter::kTBRPointLightIndex)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
        descRangeSRV[int(ForwardRootParameter::kTBRSpotLightIndex)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2);
        descRangeSRV[int(ForwardRootParameter::kTBRShadowSpotLightIndex)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 3);
        descRangeSRV[int(ForwardRootParameter::k2DTextures)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, DirectXCommon::GetInstance()->DirectXCommon::kSrvHeapDescriptorNum, 0, 1);


        CD3DX12_ROOT_PARAMETER rootparams[int(ForwardRootParameter::parameterNum)] = {};
        rootparams[int(ForwardRootParameter::kTexture)].InitAsDescriptorTable(1, &descRangeSRV[int(ForwardRootParameter::kTexture)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(ForwardRootParameter::kParticleData)].InitAsDescriptorTable(1, &descRangeSRV[int(ForwardRootParameter::kParticleData)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[(int)ForwardRootParameter::kDirectionalLights].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kDirectionalLights]);
        rootparams[(int)ForwardRootParameter::kPointLights].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kPointLights]);
        rootparams[(int)ForwardRootParameter::kAreaLights].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kAreaLights]);
        rootparams[(int)ForwardRootParameter::kSpotLights].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kSpotLights]);
        rootparams[(int)ForwardRootParameter::kShadowSpotLights].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kShadowSpotLights]);
        rootparams[(int)ForwardRootParameter::kTBRPointLightIndex].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kTBRPointLightIndex]);
        rootparams[(int)ForwardRootParameter::kTBRSpotLightIndex].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kTBRSpotLightIndex]);
        rootparams[(int)ForwardRootParameter::kTBRShadowSpotLightIndex].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kTBRShadowSpotLightIndex]);
        rootparams[int(ForwardRootParameter::kTileInformation)].InitAsDescriptorTable(1, &descRangeSRV[int(ForwardRootParameter::kTileInformation)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[(int)ForwardRootParameter::k2DTextures].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::k2DTextures]);

        rootparams[int(ForwardRootParameter::kViewProjection)].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(ForwardRootParameter::kMaterial)].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[(int)ForwardRootParameter::kTileNum].InitAsConstants(2, 2);


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

        rootSignature_[kForward]->Create(L"particleModelRootSignature", rootSignatureDesc);

    }

    {

        // 頂点レイアウト
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

        // グラフィックスパイプライン
        D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
        gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
        gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());


        gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

        gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

        gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);


        D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
        blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        blenddesc.BlendEnable = true;
        blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
        blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

        blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
        blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

        // レンダーターゲットのブレンド設定
        D3D12_RENDER_TARGET_BLEND_DESC blenddesc2{};
        blenddesc2.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        blenddesc2.BlendEnable = false;
        blenddesc2.BlendOp = D3D12_BLEND_OP_ADD;
        blenddesc2.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blenddesc2.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

        blenddesc2.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        blenddesc2.SrcBlendAlpha = D3D12_BLEND_ONE;
        blenddesc2.DestBlendAlpha = D3D12_BLEND_ZERO;

        // ブレンドステートの設定
        gpipeline.BlendState.IndependentBlendEnable = true;
        gpipeline.BlendState.RenderTarget[0] = blenddesc;
        gpipeline.BlendState.RenderTarget[1] = blenddesc;

        gpipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();

        gpipeline.InputLayout.pInputElementDescs = inputLayout;
        gpipeline.InputLayout.NumElements = _countof(inputLayout);

        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        gpipeline.NumRenderTargets = Renderer::kFRenderTargetNum;
        gpipeline.RTVFormats[int(Renderer::kColor)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor);
        gpipeline.RTVFormats[int(Renderer::kNormal)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kNormal);
        gpipeline.SampleDesc.Count = 1;

        gpipeline.pRootSignature = *rootSignature_[kForward];

        // グラフィックスパイプラインの生成
        pipelineState_[kForward]->Create(L"particleModelPipeline", gpipeline);
    }
}

void ParticleModel::Draw(CommandContext& commandContext, ParticleModelData& bufferData, const Material& material, uint32_t modelHandle)
{
    switch (Renderer::GetRenderingMode())
    {
    case Renderer::kForward:
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kParticleData), bufferData.GetGPUHandle());

        commandContext.SetConstantBuffer(static_cast<UINT>(ForwardRootParameter::kMaterial), material.GetGPUVirtualAddress());

        ModelManager::GetInstance()->DrawInstancing(commandContext, modelHandle, static_cast<UINT>(bufferData.GetDataNum()), static_cast<UINT>(ForwardRootParameter::kTexture));
        break;
    case Renderer::kDeferred:
        commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kParticleData), bufferData.GetGPUHandle());

        commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kMaterial), material.GetGPUVirtualAddress());

        ModelManager::GetInstance()->DrawInstancing(commandContext, modelHandle, static_cast<UINT>(bufferData.GetDataNum()), static_cast<UINT>(RootParameter::kTexture));
        break;
    default:
        break;
    }

    bufferData.Reset();
}