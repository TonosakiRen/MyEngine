#include "Model.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ShaderManager.h"
#include "LightManager.h"

using namespace Microsoft::WRL;

void Model::Initialize() {
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

void Model::Finalize()
{
    for (int i = 0; i < kPipelineNum;i++) {
        rootSignature_[i].reset();
        pipelineState_[i].reset();
    }
}

void Model::PreDraw(PipelineType pipelineType,CommandContext& commandContext, const ViewProjection& viewProjection, const TileBasedRendering& tileBasedRendering) {
  
    commandContext.SetPipelineState(*pipelineState_[pipelineType]);
    commandContext.SetGraphicsRootSignature(*rootSignature_[pipelineType]);

    LightManager* lightManager = LightManager::GetInstance();

    // CBVをセット（ビュープロジェクション行列）
    switch (Renderer::GetRenderingMode())
    {
    case Renderer::kForward:       

        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kDirectionalLights), lightManager->directionalLights_->structureBuffer_.GetSRV());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kPointLights), lightManager->pointLights_->structureBuffer_.GetSRV());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kAreaLights), lightManager->areaLights_->structureBuffer_.GetSRV());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kSpotLights), lightManager->spotLights_->structureBuffer_.GetSRV());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kShadowSpotLights), lightManager->shadowSpotLights_->structureBuffer_.GetSRV());
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

void Model::CreatePipeline() {
    HRESULT result = S_FALSE;
    ComPtr<IDxcBlob> vsBlob;    
    ComPtr<IDxcBlob> psBlob;    

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"ModelVS.hlsl",ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    psBlob = shaderManager->Compile(L"ModelPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    rootSignature_[kDeferred] = std::make_unique<RootSignature>();
    pipelineState_[kDeferred] = std::make_unique<PipelineState>();

    {

        // デスクリプタレンジ
        CD3DX12_DESCRIPTOR_RANGE descRangeSRV[int(RootParameter::kDescriptorRangeNum)];
        descRangeSRV[int(RootParameter::kTexture)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::parameterNum)] = {};
        rootparams[int(RootParameter::kTexture)].InitAsDescriptorTable(1, &descRangeSRV[int(RootParameter::kTexture)], D3D12_SHADER_VISIBILITY_ALL);

        rootparams[int(RootParameter::kWorldTransform)].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kViewProjection)].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kMaterial)].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);

        // スタティックサンプラー
        CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
            CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT);

        // ルートシグネチャの設定
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.pParameters = rootparams;
        rootSignatureDesc.NumParameters = _countof(rootparams);
        rootSignatureDesc.pStaticSamplers = &samplerDesc;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        rootSignature_[kDeferred]->Create(L"modelRootSignature", rootSignatureDesc);

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

        // 深度バッファのフォーマット
        gpipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();

        // 頂点レイアウトの設定
        gpipeline.InputLayout.pInputElementDescs = inputLayout;
        gpipeline.InputLayout.NumElements = _countof(inputLayout);

        // 図形の形状設定（三角形）
        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        gpipeline.NumRenderTargets = Renderer::kRenderTargetNum;
        gpipeline.RTVFormats[int(Renderer::kColor)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor);
        gpipeline.RTVFormats[int(Renderer::kNormal)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kNormal);
        gpipeline.RTVFormats[int(Renderer::kMaterial)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kMaterial);
        gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

        gpipeline.pRootSignature = *rootSignature_[kDeferred];

        // グラフィックスパイプラインの生成
        pipelineState_[kDeferred]->Create(L"modelPipeline", gpipeline);
    }
}

void Model::CreateForwardPipeline()
{
    HRESULT result = S_FALSE;
    ComPtr<IDxcBlob> vsBlob;
    ComPtr<IDxcBlob> psBlob;

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"ModelVS.hlsl", ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    psBlob = shaderManager->Compile(L"forwardPlus/FModelPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    rootSignature_[kForward] = std::make_unique<RootSignature>();
    pipelineState_[kForward] = std::make_unique<PipelineState>();

    {

        // デスクリプタレンジ
        CD3DX12_DESCRIPTOR_RANGE descRangeSRV[int(ForwardRootParameter::kDescriptorRangeNum)];
        descRangeSRV[int(ForwardRootParameter::kTexture)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ
        descRangeSRV[int(ForwardRootParameter::kDirectionalLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
        descRangeSRV[int(ForwardRootParameter::kPointLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
        descRangeSRV[int(ForwardRootParameter::kAreaLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
        descRangeSRV[int(ForwardRootParameter::kSpotLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);
        descRangeSRV[int(ForwardRootParameter::kShadowSpotLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);
        descRangeSRV[int(ForwardRootParameter::kTileInformation)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); // u0 レジスタ
        descRangeSRV[int(ForwardRootParameter::kTBRPointLightIndex)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
        descRangeSRV[int(ForwardRootParameter::kTBRSpotLightIndex)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2);
        descRangeSRV[int(ForwardRootParameter::kTBRShadowSpotLightIndex)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 3);
        descRangeSRV[int(ForwardRootParameter::k2DTextures)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, DirectXCommon::GetInstance()->DirectXCommon::kSrvHeapDescriptorNum, 0, 1);

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[int(ForwardRootParameter::parameterNum)] = {};
        rootparams[int(ForwardRootParameter::kTexture)].InitAsDescriptorTable(1, &descRangeSRV[int(ForwardRootParameter::kTexture)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[(int)ForwardRootParameter::kDirectionalLights].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kDirectionalLights]);
        rootparams[(int)ForwardRootParameter::kPointLights].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kPointLights]);
        rootparams[(int)ForwardRootParameter::kAreaLights].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kAreaLights]);
        rootparams[(int)ForwardRootParameter::kSpotLights].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kSpotLights]);
        rootparams[(int)ForwardRootParameter::kShadowSpotLights].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kShadowSpotLights]);
        rootparams[int(ForwardRootParameter::kTileInformation)].InitAsDescriptorTable(1, &descRangeSRV[int(ForwardRootParameter::kTileInformation)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[(int)ForwardRootParameter::kTBRPointLightIndex].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kTBRPointLightIndex]);
        rootparams[(int)ForwardRootParameter::kTBRSpotLightIndex].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kTBRSpotLightIndex]);
        rootparams[(int)ForwardRootParameter::kTBRShadowSpotLightIndex].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::kTBRShadowSpotLightIndex]);
        rootparams[(int)ForwardRootParameter::k2DTextures].InitAsDescriptorTable(1, &descRangeSRV[(int)ForwardRootParameter::k2DTextures]);

        rootparams[int(ForwardRootParameter::kWorldTransform)].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(ForwardRootParameter::kViewProjection)].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(ForwardRootParameter::kMaterial)].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[(int)ForwardRootParameter::kTileNum].InitAsConstants(2, 3);

        // スタティックサンプラー
        CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
            CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT);

        // ルートシグネチャの設定
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.pParameters = rootparams;
        rootSignatureDesc.NumParameters = _countof(rootparams);
        rootSignatureDesc.pStaticSamplers = &samplerDesc;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        rootSignature_[kForward]->Create(L"modelRootSignature", rootSignatureDesc);

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

        // 深度バッファのフォーマット
        gpipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();

        // 頂点レイアウトの設定
        gpipeline.InputLayout.pInputElementDescs = inputLayout;
        gpipeline.InputLayout.NumElements = _countof(inputLayout);

        // 図形の形状設定（三角形）
        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        gpipeline.NumRenderTargets = Renderer::kFRenderTargetNum;
        gpipeline.RTVFormats[int(Renderer::kColor)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor);
        gpipeline.RTVFormats[int(Renderer::kNormal)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kNormal);
        gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

        gpipeline.pRootSignature = *rootSignature_[kForward];

        // グラフィックスパイプラインの生成
        pipelineState_[kForward]->Create(L"modelPipeline", gpipeline);
    }
}

void Model::Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform, const Material& material,const uint32_t textureHandle) {


    // CBVをセット（マテリアル）
    switch (Renderer::GetRenderingMode())
    {
    case Renderer::kForward:
        // CBVをセット（ワールド行列）
        commandContext.SetConstantBuffer(static_cast<UINT>(ForwardRootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

        // CBVをセット（マテリアル）
        commandContext.SetConstantBuffer(static_cast<UINT>(ForwardRootParameter::kMaterial), material.GetGPUVirtualAddress());

        ModelManager::GetInstance()->DrawInstanced(commandContext, modelHandle, static_cast<UINT>(RootParameter::kTexture), textureHandle);
        break;
    case Renderer::kDeferred:
        // CBVをセット（ワールド行列）
        commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

        // CBVをセット（マテリアル）
        commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kMaterial), material.GetGPUVirtualAddress());

        ModelManager::GetInstance()->DrawInstanced(commandContext, modelHandle, static_cast<UINT>(RootParameter::kTexture), textureHandle);
        break;
    default:
        break;
    }
}

void Model::Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform,SkinCluster& skinCluster, const Material& material, const uint32_t textureHandle)
{
    // CBVをセット（マテリアル）
    switch (Renderer::GetRenderingMode())
    {
    case Renderer::kForward:
        // CBVをセット（ワールド行列）
        commandContext.SetConstantBuffer(static_cast<UINT>(ForwardRootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

        // CBVをセット（マテリアル）
        commandContext.SetConstantBuffer(static_cast<UINT>(ForwardRootParameter::kMaterial), material.GetGPUVirtualAddress());

        ModelManager::GetInstance()->DrawInstanced(commandContext, modelHandle, skinCluster, static_cast<UINT>(RootParameter::kTexture), textureHandle);
        break;
    case Renderer::kDeferred:
        // CBVをセット（ワールド行列）
        commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

        // CBVをセット（マテリアル）
        commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kMaterial), material.GetGPUVirtualAddress());

        ModelManager::GetInstance()->DrawInstanced(commandContext, modelHandle, skinCluster, static_cast<UINT>(RootParameter::kTexture), textureHandle);
        break;
    default:
        break;
    }
    
}