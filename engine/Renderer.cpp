#include "Renderer.h"

#include "ShaderManager.h"
#include "ImGuiManager.h"
#include "Helper.h"
#include "WinApp.h"

#include "ViewProjection.h"
#include "DirectionalLights.h"
#include "ShadowSpotLights.h"
#include "TileBasedRendering.h"
#include "TextureManager.h"
#include "Sky.h"
#include "DrawManager.h"
#include "LightManager.h"


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

    float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
    colorBuffers_[kColor] = std::make_unique<ColorBuffer>();
    colorBuffers_[kColor]->SetClearColor(clearColor);
    colorBuffers_[kColor]->Create(L"colorBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

    resultBuffer_ = std::make_unique<ColorBuffer>();
    resultBuffer_->Create(L"resultBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

    mainDepthBuffer_ = std::make_unique<DepthBuffer>();
    mainDepthBuffer_->Create(L"mainDepthBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_D32_FLOAT);

    float clearNormal[4] = { 0.0f,0.0f,0.0f,1.0f };
    colorBuffers_[kNormal] = std::make_unique<ColorBuffer>();
    colorBuffers_[kNormal]->SetClearColor(clearNormal);
    colorBuffers_[kNormal]->Create(L"normalBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT);

    colorBuffers_[kMaterial] = std::make_unique<ColorBuffer>();
    colorBuffers_[kMaterial]->SetClearColor(clearColor);
    colorBuffers_[kMaterial]->Create(L"materialBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8_UNORM);

    //ドローマネージャー
    drawManager_ = DrawManager::GetInstance();
    drawManager_->Initialize(commandContext_);

    lightManager_ = LightManager::GetInstance();

    // ブルームを初期化
    bloom_ = std::make_unique<Bloom>();
    bloom_->Initialize(colorBuffers_[kColor].get());

    postEffect_ = std::make_unique<PostEffect>();
    postEffect_->Initialize();

    deferredRenderer_ = std::make_unique<DeferredRenderer>();
    deferredRenderer_->Initialize(colorBuffers_[kColor].get(), colorBuffers_[kNormal].get(), colorBuffers_[kMaterial].get(), mainDepthBuffer_.get());

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

    wire_ = std::make_unique<Wire>();
    wire_->Initialize();

    vignette_ = std::make_unique<Vignette>();
    vignette_->Initialize(*resultBuffer_);

    smooth_ = std::make_unique<Smooth>();
    smooth_->Initialize(*resultBuffer_);
}

void Renderer::BeginFrame()
{
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Begin();
}

void Renderer::MainRender(ViewProjection& viewProjection) {

    // メインカラーバッファをレンダ―ターゲットに
    commandContext_.TransitionResource(*colorBuffers_[kColor], D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.TransitionResource(*colorBuffers_[kNormal], D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.TransitionResource(*colorBuffers_[kMaterial], D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.TransitionResource(*mainDepthBuffer_, D3D12_RESOURCE_STATE_DEPTH_WRITE);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[] = { colorBuffers_[kColor]->GetRTV(),colorBuffers_[kNormal]->GetRTV() ,colorBuffers_[kMaterial]->GetRTV() };
    commandContext_.SetRenderTargets(kRenderTargetNum, rtvHandle, mainDepthBuffer_->GetDSV());

    for (int i = 0; i < kRenderTargetNum; i++) {
        commandContext_.ClearColor(*colorBuffers_[i]);
    }

    commandContext_.TransitionResource(*resultBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.ClearColor(*resultBuffer_);

    commandContext_.ClearDepth(*mainDepthBuffer_);
    commandContext_.SetViewportAndScissorRect(0, 0, colorBuffers_[kColor]->GetWidth(), colorBuffers_[kColor]->GetHeight());

    drawManager_->AllDraw(viewProjection);
    drawManager_->ResetCalls();

}

void Renderer::DeferredRender(ViewProjection& viewProjection)
{
    wire_->AllDraw(commandContext_, viewProjection);
    tileBasedRendering_->ComputeUpdate(commandContext_, viewProjection);
    deferredRenderer_->Render(commandContext_, resultBuffer_.get(), viewProjection, *tileBasedRendering_);
    ImGui::Begin("Engine");
    if (ImGui::BeginMenu("PostEffect")) {
        ImGui::Checkbox("bloom", &isBloom_);
        ImGui::Checkbox("edge", &isEdge_);
        ImGui::Checkbox("grayScale", &isGrayScale_);
        ImGui::Checkbox("vignette", &isVignette_);
        ImGui::Checkbox("smooth", &isSmooth_);
        ImGui::EndMenu();
    }
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
    ImGui::End();
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

    //コピーできないよん
    //commandContext_.CopyBuffer(swapChain_.GetColorBuffer(), *resultBuffer_);

    //コピーしようとするとき消して
    //commandContext_.TransitionResource(*resultBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET); // ??なんでこれで警告消える？
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
    CommandQueue& commandQueue = graphics_->GetCommandQueue();

    commandQueue.Excute(commandContext_);
    swapChain_->Present();

    commandQueue.Signal();
    commandQueue.WaitForGPU();
    commandQueue.UpdateFixFPS();

    commandContext_.Reset();
}

void Renderer::Shutdown() {

    for (int i = 0; i < kRenderTargetNum; i++) {
        colorBuffers_[i].reset();
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
    wire_.reset();
    vignette_.reset();
    smooth_.reset();

    swapChain_.reset();

    drawManager_->Finalize();
    commandContext_.ShutDown();

    graphics_->Shutdown();
    ImGuiManager_->Finalize();
}


