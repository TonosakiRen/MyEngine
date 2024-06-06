#include "ParticleModel.h"
#include "ShaderManager.h"
#include "DirectXCommon.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "ModelManager.h"

using namespace Microsoft::WRL;

void ParticleModel::Initialize() {
    CreatePipeline();
}

void ParticleModel::Finalize()
{
    rootSignature_.reset();
    pipelineState_.reset();
}

void ParticleModel::PreDraw(CommandContext& commandContext, const ViewProjection& viewProjection) {
    
    commandContext.SetPipelineState(*pipelineState_);
    commandContext.SetGraphicsRootSignature(*rootSignature_);
    // CBVをセット（ビュープロジェクション行列）
    commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kViewProjection), viewProjection.GetGPUVirtualAddress());
  
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void ParticleModel::CreatePipeline() {
    ComPtr<IDxcBlob> vsBlob;
    ComPtr<IDxcBlob> psBlob;

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"ParticleVS.hlsl", ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    psBlob = shaderManager->Compile(L"ParticlePS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    rootSignature_ = std::make_unique<RootSignature>();
    pipelineState_ = std::make_unique<PipelineState>();

    {

        CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
        descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0

        CD3DX12_DESCRIPTOR_RANGE descRangeSRVTexture;
        descRangeSRVTexture.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1

        CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::parameterNum)] = {};
        rootparams[int(RootParameter::kWorldTransform)].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kViewProjection)].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kTexture)].InitAsDescriptorTable(1, &descRangeSRVTexture, D3D12_SHADER_VISIBILITY_ALL);
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

        rootSignature_->Create(rootSignatureDesc);

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

        gpipeline.pRootSignature = *rootSignature_;

        // グラフィックスパイプラインの生成
        pipelineState_->Create(gpipeline);
    }

}

void ParticleModel::Draw(CommandContext& commandContext, ParticleModelData& bufferData, const Material& material, uint32_t modelHandle)
{
 
    commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kWorldTransform), bufferData.structuredBuffer_.GetSRV());

    commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kMaterial), material.GetGPUVirtualAddress());

    ModelManager::GetInstance()->DrawInstancing(commandContext, modelHandle, static_cast<UINT>(bufferData.dataNum_), static_cast<UINT>(RootParameter::kTexture));

    bufferData.Reset();
}