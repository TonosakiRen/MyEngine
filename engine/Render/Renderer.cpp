/**
 * @file Renderer.h
 * @brief Renderingを行う
 */
#include "Render/Renderer.h"

#include "Light/ShaderManager.h"
#include "ImGuiManager.h"
#include "Graphics/Helper.h"
#include "Graphics/WinApp.h"

#include "GameComponent/ViewProjection.h"
#include "Light/DirectionalLights.h"
#include "Light/ShadowSpotLights.h"
#include "Light/TileBasedRendering.h"
#include "Texture/TextureManager.h"
#include "Sky/Sky.h"
#include "Draw/DrawManager.h"
#include "Light/LightManager.h"
#include "GPUResource/BufferManager.h"

Renderer::RenderingMode Renderer::renderingMode = Renderer::kForward;

Renderer* Renderer::GetInstance() {
    static Renderer instance;
    return &instance;
}

void Renderer::Initialize() {
     //描画デバイスを初期化
    graphics_ = DirectXCommon::GetInstance();
    graphics_->Initialize();

    // コマンドリストを初期化   
    commandContext_.Create();
    commandContext_.SetDescriptorHeap();

    bufferManager_ = BufferManager::GetInstance();

    // シェーダー関連を初期化
    ShaderManager::GetInstance()->Initialize();

    // スワップチェーンを初期化
    auto window = WinApp::GetInstance();
    swapChain_ = std::make_unique<SwapChain>();
    swapChain_->Create(window->GetHwnd());


    // メインとなるバッファを初期化
    auto& swapChainBuffer = swapChain_->GetColorBuffer();

    tmpBuffer_ = std::make_unique<ColorBuffer>();
    tmpBuffer_->Create(L"tmpBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

    resultBuffer_ = std::make_unique<ColorBuffer>();
    resultBuffer_->Create(L"resultBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

    mainDepthBuffer_ = std::make_unique<DepthBuffer>();
    mainDepthBuffer_->Create(L"mainDepthBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_D32_FLOAT);

    float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
    colorBuffers_[kColor] = std::make_unique<ColorBuffer>();
    colorBuffers_[kColor]->SetClearColor(clearColor);
    colorBuffers_[kColor]->Create(L"colorBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

    float clearNormal[4] = { 0.0f,0.0f,0.0f,1.0f };
    colorBuffers_[kNormal] = std::make_unique<ColorBuffer>();
    colorBuffers_[kNormal]->SetClearColor(clearNormal);
    colorBuffers_[kNormal]->Create(L"normalBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT);


    switch (renderingMode)
    {
    case Renderer::kForward:

        break;
    case Renderer::kDeferred:
        colorBuffers_[kMaterial] = std::make_unique<ColorBuffer>();
        colorBuffers_[kMaterial]->SetClearColor(clearColor);
        colorBuffers_[kMaterial]->Create(L"materialBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8_UNORM);

        deferredRenderer_ = std::make_unique<DeferredRenderer>();
        deferredRenderer_->Initialize(colorBuffers_[kColor].get(), colorBuffers_[kNormal].get(), colorBuffers_[kMaterial].get(), mainDepthBuffer_.get());
        break;
    default:
        break;
    }


    //ドローマネージャー
    drawManager_ = DrawManager::GetInstance();
    drawManager_->Initialize(commandContext_);

    lightManager_ = LightManager::GetInstance();

    // ブルームを初期化
    bloom_ = std::make_unique<Bloom>();
    bloom_->Initialize(colorBuffers_[kColor].get());

    postEffect_ = std::make_unique<PostEffect>();
    postEffect_->Initialize();

    edgeRenderer_ = std::make_unique<EdgeRenderer>();
    edgeRenderer_->Initialize(colorBuffers_[kColor].get(), colorBuffers_[kNormal].get(), mainDepthBuffer_.get());

    // ImGuiを初期化
    ImGuiManager_ = ImGuiManager::GetInstance();
    ImGuiManager_->Initialize(window);

    tileBasedRendering_ = std::make_unique<TileBasedRendering>();
    tileBasedRendering_->Initialize();

    transition_ = std::make_unique<Transition>();
    transition_->Initialize(*resultBuffer_);

    grayScale_ = std::make_unique<GrayScale>();
    grayScale_->Initialize(*resultBuffer_);

    hsvFilter_ = std::make_unique<HSVFilter>();
    hsvFilter_->Initialize(*resultBuffer_);

    wire_ = std::make_unique<Wire>();
    wire_->Initialize();

    vignette_ = std::make_unique<Vignette>();
    vignette_->Initialize(*resultBuffer_);

    smooth_ = std::make_unique<Smooth>();
    smooth_->Initialize(*resultBuffer_);
}

void Renderer::BeginFrame()
{

    commandContext_.Start();
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Begin();
}

void Renderer::Render(ViewProjection& viewProjection)
{
    MainRender(viewProjection);
    DeferredRender(viewProjection);
    ShadowMapRender();   
    SpotLightShadowMapRender();
    UIRender();
    EndRender();
}

void Renderer::MainRender(ViewProjection& viewProjection) {

    tileBasedRendering_->Dispatch(commandContext_, viewProjection);
    
    // メインカラーバッファをレンダ―ターゲットに
    commandContext_.TransitionResource(*colorBuffers_[kColor], D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.TransitionResource(*colorBuffers_[kNormal], D3D12_RESOURCE_STATE_RENDER_TARGET);

    switch (renderingMode)
    {
    case Renderer::kForward:

        break;
    case Renderer::kDeferred:
        commandContext_.TransitionResource(*colorBuffers_[RenderTargetType::kMaterial], D3D12_RESOURCE_STATE_RENDER_TARGET);
        break;
    default:
        break;
    }

    commandContext_.TransitionResource(*mainDepthBuffer_, D3D12_RESOURCE_STATE_DEPTH_WRITE);

    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandle;
    rtvHandle.reserve(static_cast<uint32_t>(RenderTargetType::kRenderTargetNum));
    switch (renderingMode)
    {
    case Renderer::kForward:
        rtvHandle = { colorBuffers_[kColor]->GetRTV(),colorBuffers_[kNormal]->GetRTV() };
        commandContext_.SetRenderTargets(kFRenderTargetNum, rtvHandle.data(), mainDepthBuffer_->GetDSV());
        for (int i = 0; i < kFRenderTargetNum; i++) {
            commandContext_.ClearColor(*colorBuffers_[i]);
        }
        break;
    case Renderer::kDeferred:
        rtvHandle = { colorBuffers_[kColor]->GetRTV(), colorBuffers_[kNormal]->GetRTV() ,colorBuffers_[static_cast<uint32_t>(RenderTargetType::kMaterial)]->GetRTV() };
        commandContext_.SetRenderTargets(static_cast<uint32_t>(RenderTargetType::kRenderTargetNum), rtvHandle.data(), mainDepthBuffer_->GetDSV());
        for (int i = 0; i < static_cast<uint32_t>(RenderTargetType::kRenderTargetNum); i++) {
            commandContext_.ClearColor(*colorBuffers_[i]);
        }
        break;
    default:
        break;
    }

    commandContext_.TransitionResource(*resultBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.ClearColor(*resultBuffer_);

    commandContext_.ClearDepth(*mainDepthBuffer_);
    commandContext_.SetViewportAndScissorRect(0, 0, colorBuffers_[kColor]->GetWidth(), colorBuffers_[kColor]->GetHeight());

    switch (renderingMode)
    {
    case Renderer::kForward:
        drawManager_->AllDraw(PipelineType::kForward,viewProjection,*tileBasedRendering_);
        break;
    case Renderer::kDeferred:
        drawManager_->AllDraw(PipelineType::kDeferred,viewProjection, *tileBasedRendering_);
        break;
    default:
        break;
    }
  
}

void Renderer::DeferredRender(ViewProjection& viewProjection)
{
    wire_->AllDraw(commandContext_, viewProjection);

    switch (renderingMode)
    {
    case Renderer::kForward:
        commandContext_.CopyBuffer(*resultBuffer_, *colorBuffers_[kColor]);
        break;
    case Renderer::kDeferred:
        deferredRenderer_->Render(commandContext_, resultBuffer_.get(), viewProjection, *tileBasedRendering_);
        break;
    default:
        break;
    }
#ifdef _DEBUG
    ImGui::Begin("Engine");
    if (ImGui::BeginMenu("PostEffect")) {
        ImGui::Checkbox("bloom", &isBloom_);
        ImGui::Checkbox("edge", &isEdge_);
        ImGui::Checkbox("grayScale", &isGrayScale_);
        ImGui::Checkbox("HSVFilter", &isHSVFilter);
        ImGui::Checkbox("vignette", &isVignette_);
        ImGui::Checkbox("smooth", &isSmooth_);
        ImGui::EndMenu();
    }
#endif
    if (isBloom_) {
        bloom_->Render(commandContext_, resultBuffer_.get());
    }
    if (isEdge_) {
        edgeRenderer_->Render(commandContext_,*tmpBuffer_.get(), resultBuffer_.get());
    }
    if (isGrayScale_) {
        grayScale_->Draw(*resultBuffer_.get(), *tmpBuffer_.get(), commandContext_);
    }
    if (isVignette_) {
        vignette_->Draw(*resultBuffer_.get(), *tmpBuffer_.get(), commandContext_);
    }
    if (isSmooth_) {
        smooth_->Draw(*resultBuffer_.get(), *tmpBuffer_.get(), commandContext_);
    }
    if (isHSVFilter) {
        hsvFilter_->Draw(*resultBuffer_.get(), *tmpBuffer_.get(), commandContext_);
    }
    ImGui::End();

    commandContext_.TransitionResource(*resultBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.SetViewportAndScissorRect(0, 0, resultBuffer_->GetWidth(), resultBuffer_->GetHeight());
    commandContext_.SetRenderTarget(resultBuffer_->GetRTV());

    drawManager_->PostSpriteDraw();
    drawManager_->ResetCalls();

}

void Renderer::ShadowMapRender()
{
    commandContext_.SetViewportAndScissorRect(0, 0, DirectionalLights::shadowWidth, DirectionalLights::shadowHeight);

    drawManager_->ShadowDraw();

    for (int i = 0; i < DirectionalLights::lightNum; i++) {
        commandContext_.TransitionResource(lightManager_->directionalLights_->lights_[i].shadowMap_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

void Renderer::SpotLightShadowMapRender()
{
    commandContext_.SetViewportAndScissorRect(0, 0, ShadowSpotLights::shadowWidth, ShadowSpotLights::shadowHeight);

    drawManager_->ShadowSpotLightDraw();

    for (int i = 0; i < ShadowSpotLights::lightNum; i++) {
        commandContext_.TransitionResource(lightManager_->shadowSpotLights_->lights_[i].shadowMap_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

void Renderer::UIRender()
{
    commandContext_.SetViewportAndScissorRect(0, 0, resultBuffer_->GetWidth(), resultBuffer_->GetHeight());  
    commandContext_.SetRenderTarget(resultBuffer_->GetRTV());

}

void Renderer::EndRender()
{

    transition_->Draw(*resultBuffer_, TextureManager::GetInstance()->GetSRV("white1x1.png"), commandContext_);
    commandContext_.TransitionResource(*resultBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    commandContext_.SetViewportAndScissorRect(0, 0, resultBuffer_->GetWidth(), resultBuffer_->GetHeight());
    commandContext_.TransitionResource(swapChain_->GetColorBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.SetRenderTarget(swapChain_->GetRTV());
    commandContext_.ClearColor(swapChain_->GetColorBuffer());

    postEffect_->Draw(resultBuffer_->GetSRV(), commandContext_);

    // ImGuiを描画
    ImGuiManager_->Draw(commandContext_);

    commandContext_.TransitionResource(swapChain_->GetColorBuffer(), D3D12_RESOURCE_STATE_PRESENT);
    commandContext_.Close();

    //commandList実行
    CommandQueue& commandQueue = graphics_->GetCommandQueue();

    if (!isBeginFrame_) {
        commandQueue.Signal();
        commandQueue.WaitForGPU();
        commandQueue.UpdateFixFPS();

        swapChain_->Present();

        commandContext_.Reset();
        bufferManager_->ReleaseAllResource();
    }
    else {
        isBeginFrame_ = false;
    }

    commandQueue.Excute(commandContext_);
    swapChain_->SwapBackBuffer();
    
}

void Renderer::Finalize() {


    CommandQueue& commandQueue = graphics_->GetCommandQueue();
    commandQueue.Signal();
    commandQueue.WaitForGPU();

    switch (renderingMode)
    {
    case Renderer::kForward:
        for (int i = 0; i < kFRenderTargetNum; i++) {
            colorBuffers_[i].reset();
        }
        break;
    case Renderer::kDeferred:
        for (int i = 0; i < static_cast<uint32_t>(RenderTargetType::kRenderTargetNum); i++) {
            colorBuffers_[i].reset();
        }
        break;
    default:
        break;
    }

    mainDepthBuffer_.reset();
    resultBuffer_.reset();
    deferredRenderer_.reset();
    tmpBuffer_.reset();
    edgeRenderer_.reset();
    bloom_.reset();
    postEffect_.reset();
    bloom_.reset();
    tileBasedRendering_.reset();
    transition_.reset();
    grayScale_.reset();
    hsvFilter_.reset();
    wire_.reset();
    vignette_.reset();
    smooth_.reset();

    swapChain_.reset();

    drawManager_->Finalize();
    commandContext_.Finalize();

    graphics_->Finalize();
    ImGuiManager_->Finalize();

    bufferManager_->Finalize();
}


