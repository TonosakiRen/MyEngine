/**
 * @file BlockNoise.cpp
 * @brief ブロックノイズ用Pipeline
 */
#include "Glitch/BlockNoise.h"
#include "Light/ShaderManager.h"
#include "Graphics/CommandContext.h"
#include <vector>

#include "Graphics/Helper.h"

using namespace Microsoft::WRL;

void BlockNoise::Initialize()
{

    CreatePipeline();
    blockNoiseTexture_.Create(L"blockNoiseTexture", 1024, 1024, DXGI_FORMAT_R32G32B32A32_FLOAT);
}

void BlockNoise::Render(CommandContext& commandContext)
{
    commandContext.TransitionResource(blockNoiseTexture_, D3D12_RESOURCE_STATE_RENDER_TARGET);

    commandContext.ClearColor(blockNoiseTexture_);
    commandContext.SetViewportAndScissorRect(0, 0, blockNoiseTexture_.GetWidth(), blockNoiseTexture_.GetHeight());

    commandContext.SetGraphicsRootSignature(*sRootSignature);

    commandContext.SetPipelineState(*sPipelineState);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    commandContext.SetRenderTarget(blockNoiseTexture_.GetRTV());
    commandContext.DrawInstanced(3,1, 0, 0);
    
    commandContext.TransitionResource(blockNoiseTexture_, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
}

void BlockNoise::CreatePipeline()
{
    ComPtr<IDxcBlob> vsBlob;
    ComPtr<IDxcBlob> psBlob;
    ComPtr<ID3DBlob> errorBlob;

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"PostEffectVS.hlsl", ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    psBlob = shaderManager->Compile(L"BlockNoisePS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    sRootSignature = std::make_unique<RootSignature>();
    sPipelineState = std::make_unique<PipelineState>();

    {
        // デスクリプタレンジ
        CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
        descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[2] = {};
        rootparams[0].InitAsDescriptorTable(1, &descRangeSRV);
        rootparams[1].InitAsConstants(1, 0);

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

        sRootSignature->Create(L"blockNoiseRootSignature", rootSignatureDesc);

    }

    {

        
        // グラフィックスパイプラインの流れを設定
        D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
        gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
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
        gpipeline.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT; // 0～255指定のRGBA
        gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

        gpipeline.pRootSignature = *sRootSignature;

        // グラフィックスパイプラインの生成
        sPipelineState->Create(L"blockNoisePipeline", gpipeline);
    }
}