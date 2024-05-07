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
#include "LightNumBuffer.h"
#include "TileBasedRendering.h"
#include "Transition.h"
#include "GrayScale.h"

class ViewProjection;
class DirectionalLights;
class ShadowSpotLights;

class Renderer
{
public:

    enum RenderTargetType {
        kColor,
        kNormal,
        kMaterial,

        kRenderTargetNum
    };

    static Renderer* GetInstance();

    void Initialize();
    void BeginFrame();
    void BeginMainRender();
    void EndMainRender();
    void DeferredRender(ViewProjection& viewProjection, DirectionalLights& directionalLight,PointLights& pointLights,AreaLights& areaLights, SpotLights& spotLights, ShadowSpotLights& shadowSpotLights);
    void BeginShadowMapRender(DirectionalLights& directionalLights);
    void EndShadowMapRender(DirectionalLights& directionalLights);
    void BeginSpotLightShadowMapRender(ShadowSpotLights& shadowSpotLights);
    void EndSpotLightShadowMapRender(ShadowSpotLights& shadowSpotLights);
    void BeginUIRender();
    void EndUIRender();
    void Shutdown();

    SwapChain& GetSwapChain() { return *swapChain_; }
    CommandContext& GetCommandContext() { return commandContext_; }

    void StartTransition() {
        transition_->StartTransition();
    };

    bool GetIsNextScene() {
        return transition_->GetIsNextScene();
    }

    DXGI_FORMAT GetRTVFormat(RenderTargetType rtvType) { return colorBuffers_[rtvType]->GetFormat(); }
    DXGI_FORMAT GetDSVFormat() { return mainDepthBuffer_->GetFormat(); }

    void ClearMainDepthBuffer() { commandContext_.ClearDepth(*mainDepthBuffer_); }

private:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    DirectXCommon* graphics_ = nullptr;
    std::unique_ptr<SwapChain> swapChain_;
    CommandContext commandContext_;

    std::unique_ptr<ColorBuffer> colorBuffers_[kRenderTargetNum];
    std::unique_ptr<DepthBuffer> mainDepthBuffer_;

    std::unique_ptr<ColorBuffer> resultBuffer_;

    std::unique_ptr<DeferredRenderer> deferredRenderer_;
    std::unique_ptr<EdgeRenderer> edgeRenderer_;

    std::unique_ptr<Bloom> bloom_;
    std::unique_ptr<PostEffect> postEffect_;

    std::unique_ptr<LightNumBuffer> lightNumBuffer_;

    std::unique_ptr<TileBasedRendering> tileBasedRendering_;

    std::unique_ptr<Transition> transition_;

    std::unique_ptr<GrayScale> grayScale_;;
};

