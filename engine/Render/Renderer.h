#pragma once
/**
 * @file Renderer.h
 * @brief Renderingを行う
 */
#include "Graphics/DirectXCommon.h"
#include "Graphics/SwapChain.h"
#include "Graphics/CommandContext.h"
#include "GPUResource/ColorBuffer.h"
#include "GPUResource/DepthBuffer.h"
#include "PostEffect/Bloom.h"
#include "PostEffect/PostEffect.h"

#include "Light/DeferredRenderer.h"
#include "PostEffect/EdgeRenderer.h"
#include "Light/TileBasedRendering.h"
#include "Render/Transition.h"
#include "PostEffect/GrayScale.h"
#include "PostEffect/Vignette.h"
#include "Render/Wire.h"
#include "PostEffect/Smooth.h"
#include "PostEffect/HSVFilter.h"

class ViewProjection;
class DirectionalLights;
class ShadowSpotLights;
class DrawManager;
class LightManager;
class ImGuiManager;
class BufferManager;

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



    void Initialize();
    void Finalize();

    //frame開始処理
    void BeginFrame();
    //描画
    void Render(ViewProjection& viewProjection);
    
    //遷移開始
    void StartTransition() {
        transition_->StartTransition();
    };

    //Getter
    bool GetIsNextScene() {
        return transition_->GetIsNextScene();
    }
    SwapChain& GetSwapChain() { return *swapChain_; }
    CommandContext& GetCommandContext() { return commandContext_; }
    DXGI_FORMAT GetRTVFormat(RenderTargetType rtvType) { return colorBuffers_[static_cast<uint32_t>(rtvType)]->GetFormat(); }
    DXGI_FORMAT GetDSVFormat() { return mainDepthBuffer_->GetFormat(); }
    static RenderingMode GetRenderingMode() {
        return renderingMode;
    }
private:
    //gameSceneの描画
    void MainRender(ViewProjection& viewProjection);
    //main描画の終わった後の描画
    void DeferredRender(ViewProjection& viewProjection);
    //影描画
    void ShadowMapRender();
    //スポットライト影描画
    void SpotLightShadowMapRender();
    //UI描画
    void UIRender();
    //残りの描画
    void EndRender();
private:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    static RenderingMode renderingMode;

    DirectXCommon* graphics_ = nullptr;
    DrawManager* drawManager_ = nullptr;
    LightManager* lightManager_ = nullptr;
    ImGuiManager* ImGuiManager_ = nullptr;
    BufferManager* bufferManager_ = nullptr;
    std::unique_ptr<SwapChain> swapChain_;
    CommandContext commandContext_;
    bool isBeginFrame_ = true;

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

    bool isEdge_ = false;
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

