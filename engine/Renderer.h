#pragma once

#include "DirectXCommon.h"
#include "SwapChain.h"
#include "CommandContext.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "Bloom.h"
#include "PostEffect.h"

#include "DeferredRenderer.h"
#include "EdgeRenderer.h"
#include "TileBasedRendering.h"
#include "Transition.h"
#include "GrayScale.h"
#include "Vignette.h"
#include "Wire.h"
#include "Smooth.h"
#include "HSVFilter.h"

class ViewProjection;
class DirectionalLights;
class ShadowSpotLights;
class DrawManager;
class LightManager;
class ImGuiManager;

class Renderer
{
public:

    enum RenderTargetType {
        kColor,
        kNormal,

        kFRenderTargetNum,

        kMaterial = 2,

        kRenderTargetNum
    };

    enum RenderingMode {
        kForward,
        kDeferred,

        kRenderingModeNum
    };


    static Renderer* GetInstance();


    static uint32_t time;

    void Initialize();
    void BeginFrame();

    void MainRender(ViewProjection& viewProjection);

    void DeferredRender(ViewProjection& viewProjection);
    void ShadowMapRender();
    void SpotLightShadowMapRender();

    void UIRender();
    void EndRender();
    void Shutdown();

    SwapChain& GetSwapChain() { return *swapChain_; }
    CommandContext& GetCommandContext() { return commandContext_; }

    void StartTransition() {
        transition_->StartTransition();
    };

    bool GetIsNextScene() {
        return transition_->GetIsNextScene();
    }

    DXGI_FORMAT GetRTVFormat(RenderTargetType rtvType) { return colorBuffers_[static_cast<uint32_t>(rtvType)]->GetFormat(); }
    DXGI_FORMAT GetDSVFormat() { return mainDepthBuffer_->GetFormat(); }

    void ClearMainDepthBuffer() { commandContext_.ClearDepth(*mainDepthBuffer_); }


    static RenderingMode GetRenderingMode() {
        return renderingMode;
    }

private:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    static RenderingMode renderingMode;

    DirectXCommon* graphics_ = nullptr;
    DrawManager* drawManager_ = nullptr;
    LightManager* lightManager_ = nullptr;
    ImGuiManager* ImGuiManager_ = nullptr;
    std::unique_ptr<SwapChain> swapChain_;
    CommandContext commandContext_;

    std::unique_ptr<ColorBuffer> colorBuffers_[static_cast<int>(RenderTargetType::kRenderTargetNum)];
    std::unique_ptr<DepthBuffer> mainDepthBuffer_;

    std::unique_ptr<ColorBuffer> resultBuffer_;

    std::unique_ptr<DeferredRenderer> deferredRenderer_;

    std::unique_ptr<PostEffect> postEffect_;

    std::unique_ptr<ColorBuffer> tmpBuffer_;

    std::unique_ptr<EdgeRenderer> edgeRenderer_;
    std::unique_ptr<Bloom> bloom_;
    std::unique_ptr<GrayScale> grayScale_;
    std::unique_ptr < HSVFilter> hsvFilter_;
    std::unique_ptr<Vignette> vignette_;
    std::unique_ptr<Smooth> smooth_;

    bool isEdge_ = true;
    bool isBloom_ = true;
    bool isHSVFilter = true;
    bool isGrayScale_ = false;
    bool isVignette_ = false;
    bool isSmooth_ = false;

    std::unique_ptr<TileBasedRendering> tileBasedRendering_;

    std::unique_ptr<Transition> transition_;

    std::unique_ptr<Wire> wire_;
    bool isWire_ = false;

};

