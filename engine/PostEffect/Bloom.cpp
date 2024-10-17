#include "PostEffect/Bloom.h"

#include "Light/ShaderManager.h"
#include "Graphics/CommandContext.h"
#include "Graphics/Helper.h"

#include <d3dx12.h>

using namespace Microsoft::WRL;

void Bloom::Initialize(ColorBuffer* originalTexture)
{

    luminanceTexture_.Create(L"luminanceTexture",
        originalTexture->GetWidth(),
        originalTexture->GetHeight(),
        originalTexture->GetFormat());

    gaussianBlurs_[0].Initialize(&luminanceTexture_);
    for (uint32_t i = 1; i < kMaxLevel; ++i) {
        gaussianBlurs_[i].Initialize(&gaussianBlurs_[i - 1].GetResult());
    }

    {
        CD3DX12_DESCRIPTOR_RANGE ranges[kMaxLevel]{};
        for (uint32_t i = 0; i < kMaxLevel; ++i) {
            ranges[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i);
        }

        CD3DX12_ROOT_PARAMETER rootParameters[kMaxLevel + 1]{};
        rootParameters[0].InitAsConstants(2, 0);
        for (uint32_t i = 0; i < kMaxLevel; ++i) {
            rootParameters[i + 1].InitAsDescriptorTable(1, &ranges[i]);
        }

        CD3DX12_STATIC_SAMPLER_DESC staticSamplerDesc(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        rootSignatureDesc.pStaticSamplers = &staticSamplerDesc;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        rootSignature_.Create(L"bloomRootSignature", rootSignatureDesc);
    }

    {

        DXGI_FORMAT format = originalTexture->GetFormat();

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
        psoDesc.pRootSignature = rootSignature_;

        auto shaderManager = ShaderManager::GetInstance();

        ComPtr<IDxcBlob> vs = shaderManager->Compile(L"PostEffectVS.hlsl", ShaderManager::kVertex);
        ComPtr<IDxcBlob> ps = shaderManager->Compile(L"LuminanceExtractionrPS.hlsl", ShaderManager::kPixel);
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());

        psoDesc.BlendState = Helper::BlendDisable;
        psoDesc.RasterizerState = Helper::RasterizerNoCull;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = format;
        psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.SampleDesc.Count = 1;

        luminacePipelineState_.Create(L"luminancePipeline", psoDesc);

        ps = shaderManager->Compile(L"BloomPS.hlsl", ShaderManager::kPixel);
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());

        psoDesc.BlendState = Helper::BlendAdditive;
        additivePipelineState_.Create(L"additivePipeline", psoDesc);
    }

}

void Bloom::Render(CommandContext& commandContext, ColorBuffer* originalTexture, uint32_t level)
{
    assert(level <= kMaxLevel);
    if (threshold_ == 0.0f) {
        threshold_ = 0.00001f;
    }
    if (threshold_ == 1.0f) {
        threshold_ = 1.00001f;
    }

    commandContext.TransitionResource(*originalTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(luminanceTexture_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.SetRenderTarget(luminanceTexture_.GetRTV());
    commandContext.ClearColor(luminanceTexture_);
    commandContext.SetViewportAndScissorRect(0, 0, luminanceTexture_.GetWidth(), luminanceTexture_.GetHeight());

    commandContext.SetGraphicsRootSignature(rootSignature_);


    commandContext.SetPipelineState(luminacePipelineState_);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetConstants(0, float(threshold_), float(knee_));
    commandContext.SetDescriptorTable(1, originalTexture->GetSRV());
    commandContext.DrawInstanced(3,1,0,0);

   
    for (uint32_t i = 0; i < level; ++i) {
        gaussianBlurs_[i].Render(commandContext);
    }

    commandContext.TransitionResource(*originalTexture, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.SetRenderTarget(originalTexture->GetRTV());
    commandContext.SetViewportAndScissorRect(0, 0, originalTexture->GetWidth(), originalTexture->GetHeight());

    commandContext.SetGraphicsRootSignature(rootSignature_);
    commandContext.SetPipelineState(additivePipelineState_);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetConstants(0, float(intensity_));
    for (uint32_t i = 0; i < level; ++i) {
        commandContext.SetDescriptorTable(i + 1, gaussianBlurs_[i].GetResult().GetSRV());
    }
    commandContext.DrawInstanced(3, 1, 0, 0);

}