#pragma once

#include "DirectXCommon.h"
#include "SwapChain.h"
#include "CommandContext.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "Bloom.h"
#include "PostEffect.h"

class Renderer
{
public:

    enum RenderTargetType {
        kMain,
        kNormal,

        kRenderTargetNum
    };

	static Renderer* GetInstance();

    void Initialize();
    void BeginFrame();
    void BeginMainRender();
    void EndMainRender();
    void BeginUIRender();
    void EndUIRender();
    void Shutdown();

    SwapChain& GetSwapChain() { return swapChain_; }
    CommandContext& GetCommandContext() { return commandContext_; }
    Bloom& GetBloom() { return bloom_; }

    DXGI_FORMAT GetRTVFormat(RenderTargetType rtvType) { return colorBuffers_[rtvType].GetFormat(); }
    DXGI_FORMAT GetDSVFormat() { return mainDepthBuffer_.GetFormat(); }

    void ClearMainDepthBuffer() { commandContext_.ClearDepth(mainDepthBuffer_); }

private:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;


    DirectXCommon* graphics_ = nullptr;
    SwapChain swapChain_;
    CommandContext commandContext_;

    ColorBuffer colorBuffers_[kRenderTargetNum];
    DepthBuffer mainDepthBuffer_;
    Bloom bloom_;
    PostEffect postEffect_;
};

