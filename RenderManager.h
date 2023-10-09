#pragma once

#include "Graphics.h"
#include "SwapChain.h"
#include "CommandContext.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"

class RenderManager {
public:

    static RenderManager* GetInstance();

    void Initialize();
    void Reset();
    void BeginRender();
    void EndRender();
    void Shutdown();

    SwapChain& GetSwapChain() { return swapChain_; }
    CommandContext& GetCommandContext() { return commandContexts_[swapChain_.GetBufferIndex()]; }

    DXGI_FORMAT GetSwapChainRTVFormat() const { return swapChain_.GetColorBuffer().GetFormat(); }
    DXGI_FORMAT GetMainBufferRTVFormat() const { return mainColorBuffer_.GetFormat(); }
    DXGI_FORMAT GetMainDepthDSVFormat() const { return mainDepthBuffer_.GetFormat(); }

private:

    Graphics* graphics_ = nullptr;
    SwapChain swapChain_;
    CommandContext commandContexts_[SwapChain::kNumBuffers];

    ColorBuffer mainColorBuffer_;
    DepthBuffer mainDepthBuffer_;
private:
    // シングルトン
    RenderManager() = default;
    ~RenderManager() = default;
    RenderManager(const RenderManager&) = delete;
    const RenderManager& operator=(const RenderManager&) = delete;
};