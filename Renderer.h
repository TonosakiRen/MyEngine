#pragma once

#include "DirectXCommon.h"
#include "SwapChain.h"
#include "CommandContext.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "Bloom.h"

class Renderer
{
public:

	static Renderer* GetInstance();

    void Initialize();
    void Reset();
    void BeginRender();
    void EndRender();
    void Shutdown();

    SwapChain& GetSwapChain() { return swapChain_; }
    CommandContext& GetCommandContext() { return commandContexts_[swapChain_.GetBufferIndex()]; }
    Bloom& GetBloom() { return bloom; }

    DXGI_FORMAT GetSwapChainRTVFormat() const { return swapChain_.GetColorBuffer().GetFormat(); }
    DXGI_FORMAT GetMainBufferRTVFormat() const { return mainColorBuffer_.GetFormat(); }
    DXGI_FORMAT GetMainDepthDSVFormat() const { return mainDepthBuffer_.GetFormat(); }

private:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void InitializePostEffect();

    DirectXCommon* graphics_ = nullptr;
    SwapChain swapChain_;
    CommandContext commandContexts_[SwapChain::kNumBuffers];

    ColorBuffer mainColorBuffer_;
    DepthBuffer mainDepthBuffer_;
    RootSignature postEffectRootSignature_;
    PipelineState postEffectPipelineState_;
    Bloom bloom;
};

