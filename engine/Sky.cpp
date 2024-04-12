#include "Sky.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ShaderManager.h"
#include "ImGuiManager.h"
#include "Renderer.h"

using namespace Microsoft::WRL;

CommandContext* Sky::commandContext_ = nullptr;
std::unique_ptr<RootSignature> Sky::rootSignature_;
std::unique_ptr<PipelineState> Sky::pipelineState_;

std::unique_ptr<Voronoi> Sky::voronoi_;

bool Sky::isCreateVoronoi_ = false;

Vector4 Sky::topHSVA_ = { 0.790f,1.0f,0.06f,1.0f };
Vector4 Sky::bottomHSVA_ = { 0.220f,1.0f,0.06f,1.0f };


void Sky::StaticInitialize() {
    CreatePipeline();
}

void Sky::Finalize()
{
    rootSignature_.reset();
    pipelineState_.reset();
    voronoi_.reset();
}

void Sky::PreDraw(CommandContext* commandContext, const ViewProjection& viewProjection) {
    assert(Sky::commandContext_ == nullptr);

    if (!isCreateVoronoi_) {
        isCreateVoronoi_ = true;
        voronoi_ = std::make_unique<Voronoi>();
        voronoi_->Initialize(pointNum_);
        voronoi_->Render(*commandContext);
    }
    commandContext_ = commandContext;

    commandContext_->SetPipelineState(*pipelineState_);
    commandContext_->SetGraphicsRootSignature(*rootSignature_);

    // CBVをセット（ビュープロジェクション行列）
    commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kViewProjection), viewProjection.GetGPUVirtualAddress());

    ImGui::Begin("Sky");
    ImGui::DragFloat4("HSVA TOP COLOR", &topHSVA_.x, 0.01f,0.0f,1.0f);
    ImGui::DragFloat4("HSVA BOTTOM COLOR", &bottomHSVA_.x, 0.01f, 0.0f, 1.0f);
    ImGui::End();

    commandContext_->SetConstants(static_cast<UINT>(RootParameter::kTopColor), topHSVA_.x, topHSVA_.y, topHSVA_.z, topHSVA_.w);
    commandContext_->SetConstants(static_cast<UINT>(RootParameter::kBottomColor), bottomHSVA_.x, bottomHSVA_.y, bottomHSVA_.z, bottomHSVA_.w);

    commandContext_->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Sky::PostDraw() {
    commandContext_ = nullptr;
}

void Sky::CreatePipeline() {
    HRESULT result = S_FALSE;
    ComPtr<IDxcBlob> vsBlob;    
    ComPtr<IDxcBlob> psBlob;    

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"SkyVS.hlsl",ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    psBlob = shaderManager->Compile(L"SkyPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    rootSignature_ = std::make_unique<RootSignature>();
    pipelineState_ = std::make_unique<PipelineState>();

    {

        // デスクリプタレンジ
        CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
        descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::parameterNum)] = {};
        rootparams[int(RootParameter::kWorldTransform)].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kViewProjection)].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kVoronoiTexture)].InitAsDescriptorTable(1, &descRangeSRV);
        rootparams[int(RootParameter::kTopColor)].InitAsConstants(4,2, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kBottomColor)].InitAsConstants(4,3, 0, D3D12_SHADER_VISIBILITY_ALL);

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

        gpipeline.pRootSignature = *rootSignature_;

        // グラフィックスパイプラインの生成
        pipelineState_->Create(gpipeline);
    }
}

void Sky::Draw(uint32_t modelHandle, const WorldTransform& worldTransform) {

    // CBVをセット（ワールド行列）
    commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kWorldTransform),worldTransform.GetGPUVirtualAddress());

    ModelManager::GetInstance()->DrawInstanced(commandContext_, modelHandle,UINT(RootParameter::kVoronoiTexture),voronoi_->GetResult().GetSRV());
}
