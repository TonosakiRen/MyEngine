#include "GaussianBlur.h"
#include "ShaderManager.h"
#include "CommandContext.h"

#include "Helper.h"

using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* GaussianBlur::sCommandList = nullptr;
uint32_t GaussianBlur::gbInstanceCount = 0;

GaussianBlur::GaussianBlur() {
    gbInstanceCount++;
}

GaussianBlur::~GaussianBlur()
{
    assert(gbInstanceCount > 0);
    gbInstanceCount--;
}

void GaussianBlur::Initialize(ColorBuffer* originalTexture)
{
    assert(originalTexture);

    InitializeGraphicsPipeline();

    originalTexture_ = originalTexture;
    horizontalBlurTexture_.Create(
        originalTexture_->GetWidth() / 2,
        originalTexture_->GetHeight(),
        originalTexture_->GetFormat());

    verticalBlurTexture_.Create(
        originalTexture_->GetWidth() / 2,
        originalTexture_->GetHeight() / 2,
        originalTexture_->GetFormat());

    constantBuffer_.Create(sizeof(weights_));

    UpdateWeightTable(1.0f);
}

void GaussianBlur::Render(CommandContext& commandContext)
{
    commandContext.TransitionResource(*originalTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(horizontalBlurTexture_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.SetRenderTarget(horizontalBlurTexture_.GetRTV());
    commandContext.ClearColor(horizontalBlurTexture_);
    commandContext.SetViewportAndScissorRect(0, 0, horizontalBlurTexture_.GetWidth(), horizontalBlurTexture_.GetHeight());

    commandContext.SetGraphicsRootSignature(*sRootSignature);


    commandContext.SetPipelineState(*sHorizontalBlurPipelineState);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetDescriptorTable(0, originalTexture_->GetSRV());
    commandContext.SetConstantBuffer(1, constantBuffer_.GetGPUVirtualAddress());
    commandContext.DrawInstanced(3, 1, 0, 0);

    commandContext.TransitionResource(horizontalBlurTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(verticalBlurTexture_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.ClearColor(verticalBlurTexture_);
    commandContext.SetRenderTarget(verticalBlurTexture_.GetRTV());
    commandContext.SetViewportAndScissorRect(0, 0, verticalBlurTexture_.GetWidth(), verticalBlurTexture_.GetHeight());

    commandContext.SetGraphicsRootSignature(*sRootSignature);
    commandContext.SetPipelineState(*sVerticalBlurPipelineState);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetDescriptorTable(0, horizontalBlurTexture_.GetSRV());
    commandContext.SetConstantBuffer(1, constantBuffer_.GetGPUVirtualAddress());
    commandContext.DrawInstanced(3, 1, 0, 0);

    commandContext.TransitionResource(verticalBlurTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void GaussianBlur::InitializeGraphicsPipeline()
{
    ComPtr<IDxcBlob> horizontalVsBlob;
    ComPtr<IDxcBlob> verticalVsBlob;
    ComPtr<IDxcBlob> psBlob;
    ComPtr<ID3DBlob> errorBlob;

    auto shaderManager = ShaderManager::GetInstance();

    horizontalVsBlob = shaderManager->Compile(L"HorizontalGaussianBlurVS.hlsl", ShaderManager::kVertex);
    assert(horizontalVsBlob != nullptr);

    verticalVsBlob = shaderManager->Compile(L"VerticalGaussianBlurVS.hlsl", ShaderManager::kVertex);
    assert(verticalVsBlob != nullptr);

    psBlob = shaderManager->Compile(L"GaussianBlurPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    sRootSignature = std::make_unique<RootSignature>();
    sHorizontalBlurPipelineState = std::make_unique<PipelineState>();
    sVerticalBlurPipelineState = std::make_unique<PipelineState>();

    {
        // デスクリプタレンジ
        CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
        descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[2] = {};
        rootparams[0].InitAsDescriptorTable(1, &descRangeSRV);
        rootparams[1].InitAsConstantBufferView(0);

        // スタティックサンプラー
        CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
            CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

        // ルートシグネチャの設定
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.pParameters = rootparams;
        rootSignatureDesc.NumParameters = _countof(rootparams);
        rootSignatureDesc.pStaticSamplers = &samplerDesc;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        sRootSignature->Create(rootSignatureDesc);

    }

    {

        // グラフィックスパイプラインの流れを設定
        D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
        gpipeline.VS = CD3DX12_SHADER_BYTECODE(horizontalVsBlob->GetBufferPointer(), horizontalVsBlob->GetBufferSize());
        gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());

        // サンプルマスク
        gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
        // ラスタライザステート
        gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

        // ブレンドステートの設定
        gpipeline.BlendState = Helper::BlendDisable;

        // 深度バッファのフォーマット
        gpipeline.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

        // 図形の形状設定（三角形）
        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        gpipeline.NumRenderTargets = 1;                            // 描画対象は1つ
        gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
        gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

        gpipeline.pRootSignature = *sRootSignature;

        // グラフィックスパイプラインの生成
        sHorizontalBlurPipelineState->Create(gpipeline);

        gpipeline.VS = CD3DX12_SHADER_BYTECODE(verticalVsBlob->GetBufferPointer(), verticalVsBlob->GetBufferSize());

        // グラフィックスパイプラインの生成
        sVerticalBlurPipelineState->Create(gpipeline);
    }
}

void GaussianBlur::UpdateWeightTable(float blurPower) {
    float total = 0;
    for (uint32_t i = 0; i < kNumWeights; ++i) {
        weights_[i] = std::exp(-0.5f * float(i * i) / blurPower);
        total += 2.0f * weights_[i];
    }
    total = 1.0f / total;
    for (uint32_t i = 0; i < kNumWeights; ++i) {
        weights_[i] *= total;
    }
    constantBuffer_.Copy(weights_, sizeof(weights_));
}