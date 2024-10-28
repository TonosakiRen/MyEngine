#include "Mesh/MeshletModel.h"
#include "Texture/TextureManager.h"
#include "Model/ModelManager.h"
#include "Light/ShaderManager.h"
#include "Light/LightManager.h"

using namespace Microsoft::WRL;

void MeshletModel::Initialize() {
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

void MeshletModel::Finalize()
{
    for (int i = 0; i < kPipelineNum; i++) {
        rootSignature_[i].reset();
        pipelineState_[i].reset();
    }
}

void MeshletModel::PreDraw(PipelineType pipelineType,CommandContext& commandContext, const ViewProjection& viewProjection, const ViewProjection& cullingViewProjection, const TileBasedRendering& tileBasedRendering) {
  
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
        commandContext.SetConstantBuffer(static_cast<UINT>(ForwardRootParameter::kFrustum), cullingViewProjection.GetFrustumGPUVirtualAddress());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kTileInformation), tileBasedRendering.GetTileInformationGPUHandle());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kTBRPointLightIndex), tileBasedRendering.GetPointLightIndexGPUHandle());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kTBRSpotLightIndex), tileBasedRendering.GetSpotLightIndexGPUHandle());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::kTBRShadowSpotLightIndex), tileBasedRendering.GetShadowSpotLightIndexGPUHandle());
        commandContext.SetDescriptorTable(static_cast<UINT>(ForwardRootParameter::k2DTextures), DirectXCommon::GetInstance()->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).GetDiscriptorStartHandle());
        commandContext.SetConstants(static_cast<UINT>(ForwardRootParameter::kTileNum), TileBasedRendering::kTileWidthNum, TileBasedRendering::kTileHeightNum);
        break;
    case Renderer::kDeferred:
        commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kViewProjection), viewProjection.GetGPUVirtualAddress());
        commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kFrustum), cullingViewProjection.GetFrustumGPUVirtualAddress());
        break;
    default:
        break;
    }

    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void MeshletModel::CreatePipeline() {
    HRESULT result = S_FALSE;
    ComPtr<IDxcBlob> msBlob;    
    ComPtr<IDxcBlob> psBlob;   
    ComPtr<IDxcBlob> asBlob;

    auto shaderManager = ShaderManager::GetInstance();

    asBlob = shaderManager->Compile(L"MeshTestAS.hlsl", ShaderManager::kAmplification);
    assert(asBlob != nullptr);

    msBlob = shaderManager->Compile(L"MeshTestMS.hlsl",ShaderManager::kMesh);
    assert(msBlob != nullptr);

    psBlob = shaderManager->Compile(L"MeshTestPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    rootSignature_[kDeferred] = std::make_unique<RootSignature>();
    pipelineState_[kDeferred] = std::make_unique<PipelineState>();

    {

        // デスクリプタレンジ
        CD3DX12_DESCRIPTOR_RANGE descRangeSRV[int(RootParameter::kDescriptorRangeNum)];
        descRangeSRV[int(RootParameter::kTexture)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        descRangeSRV[int(RootParameter::kVertices)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
        descRangeSRV[int(RootParameter::kMeshlets)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
        descRangeSRV[int(RootParameter::kUniqueVertexIndices)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
        descRangeSRV[int(RootParameter::kPrimitiveIndices)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);
        descRangeSRV[int(RootParameter::kCullData)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::parameterNum)] = {};
        rootparams[int(RootParameter::kTexture)].InitAsDescriptorTable(1, &descRangeSRV[int(RootParameter::kTexture)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kVertices)].InitAsDescriptorTable(1, &descRangeSRV[int(RootParameter::kVertices)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kMeshlets)].InitAsDescriptorTable(1, &descRangeSRV[int(RootParameter::kMeshlets)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kUniqueVertexIndices)].InitAsDescriptorTable(1, &descRangeSRV[int(RootParameter::kUniqueVertexIndices)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kPrimitiveIndices)].InitAsDescriptorTable(1, &descRangeSRV[int(RootParameter::kPrimitiveIndices)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kCullData)].InitAsDescriptorTable(1, &descRangeSRV[int(RootParameter::kCullData)], D3D12_SHADER_VISIBILITY_ALL);


        rootparams[int(RootParameter::kWorldTransform)].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kViewProjection)].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kMaterial)].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kFrustum)].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);

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

        rootSignature_[kDeferred]->Create(L"meshletRootSignature", rootSignatureDesc);

    }

    {
        D3DX12_MESH_SHADER_PIPELINE_STATE_DESC pipeline = {};
        pipeline.AS = CD3DX12_SHADER_BYTECODE(asBlob->GetBufferPointer(), asBlob->GetBufferSize());
        pipeline.MS = CD3DX12_SHADER_BYTECODE(msBlob->GetBufferPointer(), msBlob->GetBufferSize());
        pipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());

        // サンプルマスク
        pipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
        // ラスタライザステート
        pipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        //  デプスステンシルステート
        pipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

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
        pipeline.BlendState.IndependentBlendEnable = true;
        pipeline.BlendState.RenderTarget[0] = blenddesc;
        pipeline.BlendState.RenderTarget[1] = blenddesc;

        // 深度バッファのフォーマット
        pipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();


        // 図形の形状設定（三角形）
        pipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        pipeline.NumRenderTargets = Renderer::kFRenderTargetNum;
        pipeline.RTVFormats[int(Renderer::kColor)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor);
        pipeline.RTVFormats[int(Renderer::kNormal)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kNormal);
        pipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

        pipeline.pRootSignature = *rootSignature_[kDeferred];

        CD3DX12_PIPELINE_MESH_STATE_STREAM psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(pipeline);

        D3D12_PIPELINE_STATE_STREAM_DESC streamDesc = {};
        streamDesc.pPipelineStateSubobjectStream = &psoStream;
        streamDesc.SizeInBytes = sizeof(psoStream);

        // グラフィックスパイプラインの生成
        pipelineState_[kDeferred]->Create(L"meshletPipeline", streamDesc);
    }
}

void MeshletModel::CreateForwardPipeline()
{
    HRESULT result = S_FALSE;
    ComPtr<IDxcBlob> msBlob;
    ComPtr<IDxcBlob> psBlob;
    ComPtr<IDxcBlob> asBlob;

    auto shaderManager = ShaderManager::GetInstance();

    asBlob = shaderManager->Compile(L"MeshTestAS.hlsl", ShaderManager::kAmplification);
    assert(asBlob != nullptr);

    msBlob = shaderManager->Compile(L"MeshTestMS.hlsl", ShaderManager::kMesh);
    assert(msBlob != nullptr);

    psBlob = shaderManager->Compile(L"forwardPlus/MeshTestPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    rootSignature_[kForward] = std::make_unique<RootSignature>();
    pipelineState_[kForward] = std::make_unique<PipelineState>();

    {

        // デスクリプタレンジ
        CD3DX12_DESCRIPTOR_RANGE descRangeSRV[int(ForwardRootParameter::kDescriptorRangeNum)];
        descRangeSRV[int(ForwardRootParameter::kTexture)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        descRangeSRV[int(ForwardRootParameter::kVertices)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
        descRangeSRV[int(ForwardRootParameter::kMeshlets)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
        descRangeSRV[int(ForwardRootParameter::kUniqueVertexIndices)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
        descRangeSRV[int(ForwardRootParameter::kPrimitiveIndices)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);
        descRangeSRV[int(ForwardRootParameter::kCullData)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);
        descRangeSRV[int(ForwardRootParameter::kDirectionalLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6);
        descRangeSRV[int(ForwardRootParameter::kPointLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 7);
        descRangeSRV[int(ForwardRootParameter::kAreaLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 8);
        descRangeSRV[int(ForwardRootParameter::kSpotLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 9);
        descRangeSRV[int(ForwardRootParameter::kShadowSpotLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 10);
        descRangeSRV[int(ForwardRootParameter::kTileInformation)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
        descRangeSRV[int(ForwardRootParameter::kTBRPointLightIndex)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
        descRangeSRV[int(ForwardRootParameter::kTBRSpotLightIndex)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2);
        descRangeSRV[int(ForwardRootParameter::kTBRShadowSpotLightIndex)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 3);
        descRangeSRV[int(ForwardRootParameter::k2DTextures)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, DirectXCommon::GetInstance()->DirectXCommon::kSrvHeapDescriptorNum, 0, 1);

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[int(ForwardRootParameter::parameterNum)] = {};
        rootparams[int(ForwardRootParameter::kTexture)].InitAsDescriptorTable(1, &descRangeSRV[int(ForwardRootParameter::kTexture)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(ForwardRootParameter::kVertices)].InitAsDescriptorTable(1, &descRangeSRV[int(ForwardRootParameter::kVertices)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(ForwardRootParameter::kMeshlets)].InitAsDescriptorTable(1, &descRangeSRV[int(ForwardRootParameter::kMeshlets)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(ForwardRootParameter::kUniqueVertexIndices)].InitAsDescriptorTable(1, &descRangeSRV[int(ForwardRootParameter::kUniqueVertexIndices)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(ForwardRootParameter::kPrimitiveIndices)].InitAsDescriptorTable(1, &descRangeSRV[int(ForwardRootParameter::kPrimitiveIndices)], D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(ForwardRootParameter::kCullData)].InitAsDescriptorTable(1, &descRangeSRV[int(ForwardRootParameter::kCullData)], D3D12_SHADER_VISIBILITY_ALL);
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
        rootparams[int(ForwardRootParameter::kMeshletInfo)].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(ForwardRootParameter::kFrustum)].InitAsConstantBufferView(4, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[(int)ForwardRootParameter::kTileNum].InitAsConstants(2, 5);

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

        rootSignature_[kForward]->Create(L"meshletRootSignature", rootSignatureDesc);

    }

    {
        D3DX12_MESH_SHADER_PIPELINE_STATE_DESC pipeline = {};
        pipeline.AS = CD3DX12_SHADER_BYTECODE(asBlob->GetBufferPointer(), asBlob->GetBufferSize());
        pipeline.MS = CD3DX12_SHADER_BYTECODE(msBlob->GetBufferPointer(), msBlob->GetBufferSize());
        pipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());

        // サンプルマスク
        pipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
        // ラスタライザステート
        pipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        //  デプスステンシルステート
        pipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

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
        pipeline.BlendState.IndependentBlendEnable = true;
        pipeline.BlendState.RenderTarget[0] = blenddesc;
        pipeline.BlendState.RenderTarget[1] = blenddesc;

        // 深度バッファのフォーマット
        pipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();


        // 図形の形状設定（三角形）
        pipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        pipeline.NumRenderTargets = Renderer::kRenderTargetNum;
        pipeline.RTVFormats[int(Renderer::kColor)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor);
        pipeline.RTVFormats[int(Renderer::kNormal)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kNormal);
        pipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

        pipeline.pRootSignature = *rootSignature_[kForward];

        CD3DX12_PIPELINE_MESH_STATE_STREAM psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(pipeline);

        D3D12_PIPELINE_STATE_STREAM_DESC streamDesc = {};
        streamDesc.pPipelineStateSubobjectStream = &psoStream;
        streamDesc.SizeInBytes = sizeof(psoStream);

        // グラフィックスパイプラインの生成
        pipelineState_[kForward]->Create(L"meshletPipeline", streamDesc);
    }
}

void MeshletModel::Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform, const Material& material,const uint32_t textureHandle) {

    const ModelData& modelItem = ModelManager::GetInstance()->ModelManager::GetModelData(modelHandle);

    // CBVをセット（マテリアル）
    switch (Renderer::GetRenderingMode())
    {
    case Renderer::kForward:
        // CBVをセット（ワールド行列）
        commandContext.SetConstantBuffer(static_cast<UINT>(ForwardRootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

        // CBVをセット（マテリアル）
        commandContext.SetConstantBuffer(static_cast<UINT>(ForwardRootParameter::kMaterial), material.GetGPUVirtualAddress());

        for (const auto& mesh : modelItem.meshes) {
            // srvセット
            if (mesh.GetUv() != 0) {
                TextureManager::GetInstance()->SetDescriptorTable(commandContext, UINT(ForwardRootParameter::kTexture), mesh.GetUv());
            }
            else {
                TextureManager::GetInstance()->SetDescriptorTable(commandContext, UINT(ForwardRootParameter::kTexture), textureHandle);
            }

            //頂点セット
            commandContext.SetDescriptorTable(UINT(ForwardRootParameter::kVertices), mesh.vertexBuffer_.GetSRV());
            //Meshletセット
            commandContext.SetDescriptorTable(UINT(ForwardRootParameter::kMeshlets), mesh.meshletBuffer_.GetSRV());
            commandContext.SetDescriptorTable(UINT(ForwardRootParameter::kPrimitiveIndices), mesh.primitiveIndicesBuffer_.GetSRV());
            commandContext.SetDescriptorTable(UINT(ForwardRootParameter::kUniqueVertexIndices), mesh.uniqueVertexIndexBuffer_.GetSRV());
            commandContext.SetDescriptorTable(UINT(ForwardRootParameter::kCullData), mesh.cullDataBuffer_.GetSRV());
            commandContext.SetConstantBuffer(UINT(ForwardRootParameter::kMeshletInfo), mesh.meshletInfo_.GetGPUVirtualAddress());

            // 描画コマンド
            commandContext.DispatchMesh(uint32_t((mesh.meshlets_.size() + 32 - 1) / 32), 1, 1);
        }
        break;
    case Renderer::kDeferred:
        // CBVをセット（ワールド行列）
        commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

        // CBVをセット（マテリアル）
        commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kMaterial), material.GetGPUVirtualAddress());

        for (const auto& mesh : modelItem.meshes) {
            // srvセット
            if (mesh.GetUv() != 0) {
                TextureManager::GetInstance()->SetDescriptorTable(commandContext, UINT(RootParameter::kTexture), mesh.GetUv());
            }
            else {
                TextureManager::GetInstance()->SetDescriptorTable(commandContext, UINT(RootParameter::kTexture), textureHandle);
            }

            //頂点セット
            commandContext.SetDescriptorTable(UINT(RootParameter::kVertices), mesh.vertexBuffer_.GetSRV());
            //Meshletセット
            commandContext.SetDescriptorTable(UINT(RootParameter::kMeshlets), mesh.meshletBuffer_.GetSRV());
            commandContext.SetDescriptorTable(UINT(RootParameter::kPrimitiveIndices), mesh.primitiveIndicesBuffer_.GetSRV());
            commandContext.SetDescriptorTable(UINT(RootParameter::kUniqueVertexIndices), mesh.uniqueVertexIndexBuffer_.GetSRV());
            commandContext.SetDescriptorTable(UINT(RootParameter::kCullData), mesh.cullDataBuffer_.GetSRV());
            commandContext.SetConstantBuffer(UINT(RootParameter::kMeshletInfo), mesh.meshletInfo_.GetGPUVirtualAddress());

            // 描画コマンド
            commandContext.DispatchMesh(uint32_t((mesh.meshlets_.size() + 32 - 1) / 32), 1, 1);
        }
        break;
    default:
        break;
    }
}

void MeshletModel::Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform, SkinCluster& skinCluster, const Material& material, const uint32_t textureHandle)
{
    const ModelData& modelItem = ModelManager::GetInstance()->ModelManager::GetModelData(modelHandle);

    // CBVをセット（マテリアル）
    switch (Renderer::GetRenderingMode())
    {
    case Renderer::kForward:
        // CBVをセット（ワールド行列）
        commandContext.SetConstantBuffer(static_cast<UINT>(ForwardRootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

        // CBVをセット（マテリアル）
        commandContext.SetConstantBuffer(static_cast<UINT>(ForwardRootParameter::kMaterial), material.GetGPUVirtualAddress());

        for (const auto& mesh : modelItem.meshes) {
            // srvセット
            if (mesh.GetUv() != 0) {
                TextureManager::GetInstance()->SetDescriptorTable(commandContext, UINT(ForwardRootParameter::kTexture), mesh.GetUv());
            }
            else {
                TextureManager::GetInstance()->SetDescriptorTable(commandContext, UINT(ForwardRootParameter::kTexture), textureHandle);
            }

            //頂点セット
            commandContext.TransitionResource(skinCluster.GetSkinnedVertices(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            commandContext.SetDescriptorTable(UINT(ForwardRootParameter::kVertices), skinCluster.GetSkinnedVerticesUAV());
            //Meshletセット
            commandContext.SetDescriptorTable(UINT(ForwardRootParameter::kMeshlets), mesh.meshletBuffer_.GetSRV());
            commandContext.SetDescriptorTable(UINT(ForwardRootParameter::kPrimitiveIndices), mesh.primitiveIndicesBuffer_.GetSRV());
            commandContext.SetDescriptorTable(UINT(ForwardRootParameter::kUniqueVertexIndices), mesh.uniqueVertexIndexBuffer_.GetSRV());
            commandContext.SetDescriptorTable(UINT(ForwardRootParameter::kCullData), mesh.cullDataBuffer_.GetSRV());
            commandContext.SetConstantBuffer(UINT(ForwardRootParameter::kMeshletInfo), mesh.meshletInfo_.GetGPUVirtualAddress());

            // 描画コマンド
            commandContext.DispatchMesh(uint32_t((mesh.meshlets_.size() + 32 - 1) / 32), 1, 1);
        }

        break;
    case Renderer::kDeferred:
        // CBVをセット（ワールド行列）
        commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

        // CBVをセット（マテリアル）
        commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kMaterial), material.GetGPUVirtualAddress());

        for (const auto& mesh : modelItem.meshes) {
            // srvセット
            if (mesh.GetUv() != 0) {
                TextureManager::GetInstance()->SetDescriptorTable(commandContext, UINT(RootParameter::kTexture), mesh.GetUv());
            }
            else {
                TextureManager::GetInstance()->SetDescriptorTable(commandContext, UINT(RootParameter::kTexture), textureHandle);
            }

            //頂点セット
            commandContext.TransitionResource(skinCluster.GetSkinnedVertices(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            commandContext.SetDescriptorTable(UINT(RootParameter::kVertices), skinCluster.GetSkinnedVerticesUAV());
            //Meshletセット
            commandContext.SetDescriptorTable(UINT(RootParameter::kMeshlets), mesh.meshletBuffer_.GetSRV());
            commandContext.SetDescriptorTable(UINT(RootParameter::kPrimitiveIndices), mesh.primitiveIndicesBuffer_.GetSRV());
            commandContext.SetDescriptorTable(UINT(RootParameter::kUniqueVertexIndices), mesh.uniqueVertexIndexBuffer_.GetSRV());
            commandContext.SetDescriptorTable(UINT(RootParameter::kCullData), mesh.cullDataBuffer_.GetSRV());
            commandContext.SetConstantBuffer(UINT(RootParameter::kMeshletInfo), mesh.meshletInfo_.GetGPUVirtualAddress());

            // 描画コマンド
            commandContext.DispatchMesh(uint32_t((mesh.meshlets_.size() + 32 - 1) / 32), 1, 1);
        }
        break;
    default:
        break;
    }
}
