#include "Renderer.h"

#include "ShaderManager.h"
#include "ImGuiManager.h"
#include "Helper.h"
#include "WinApp.h"

Renderer* Renderer::GetInstance() {
    static Renderer instance;
    return &instance;
}

void Renderer::Initialize() {
    // 描画デバイスを初期化
    graphics_ = DirectXCommon::GetInstance();
    graphics_->Initialize();

    // コマンドリストを初期化   
    commandContext_.Create();
    commandContext_.SetDescriptorHeap();

    // シェーダー関連を初期化
    ShaderManager::GetInstance()->Initialize();

    // スワップチェーンを初期化
    auto window = WinApp::GetInstance();
    swapChain_.Create(window->GetHwnd());

    // メインとなるバッファを初期化
    auto& swapChainBuffer = swapChain_.GetColorBuffer();
    float clearColor[4] = { 0.3f,0.1f,0.3f,0.0f };
    colorBuffers_[kMain].SetClearColor(clearColor);
    colorBuffers_[kMain].Create(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
    
    mainDepthBuffer_.Create(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_D32_FLOAT);

    float clearNormal[4] = { 0.0f,0.0f,0.0f,1.0f };
    colorBuffers_[kNormal].SetClearColor(clearNormal);
    colorBuffers_[kNormal].Create(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT);

    // ブルームを初期化
    bloom_.Initialize(&colorBuffers_[kMain]);
    postEffect_.Initialize();

    // ImGuiを初期化
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Initialize(window);
}

void Renderer::BeginFrame()
{
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Begin();
}

void Renderer::BeginMainRender() {

    // メインカラーバッファをレンダ―ターゲットに
    commandContext_.TransitionResource(colorBuffers_[kMain], D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.TransitionResource(colorBuffers_[kNormal], D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.TransitionResource(mainDepthBuffer_, D3D12_RESOURCE_STATE_DEPTH_WRITE);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[] = { colorBuffers_[kMain].GetRTV(),colorBuffers_[kNormal].GetRTV() };
    commandContext_.SetRenderTargets(kRenderTargetNum, rtvHandle, mainDepthBuffer_.GetDSV());
    //commandContext_.SetRenderTarget(mainColorBuffer_.GetRTV(), mainDepthBuffer_.GetDSV());
    for (int i = 0; i < kRenderTargetNum;i++) {
        commandContext_.ClearColor(colorBuffers_[i]);
    }
    commandContext_.ClearDepth(mainDepthBuffer_);
    commandContext_.SetViewportAndScissorRect(0, 0, colorBuffers_[kMain].GetWidth(), colorBuffers_[kMain].GetHeight());
}

void Renderer::EndMainRender() {

    //深度Bufferを読み取りにする
    commandContext_.TransitionResource(mainDepthBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // メインカラーバッファにブルームをかける
    //bloom_.Render(commandContext_);

    commandContext_.TransitionResource(colorBuffers_[kNormal], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    commandContext_.TransitionResource(colorBuffers_[kMain], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext_.TransitionResource(swapChain_.GetColorBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void Renderer::BeginUIRender()
{

    commandContext_.SetViewportAndScissorRect(0, 0, colorBuffers_[kMain].GetWidth(), colorBuffers_[kMain].GetHeight());

    commandContext_.SetRenderTarget(swapChain_.GetRTV());
    commandContext_.ClearColor(swapChain_.GetColorBuffer());

    PostEffect::PreDraw(commandContext_);
    postEffect_.Draw(colorBuffers_[kNormal].GetSRV());
    PostEffect::PostDraw();
}

void Renderer::EndUIRender()
{

    // ImGuiを描画
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Draw(commandContext_);

    commandContext_.TransitionResource(swapChain_.GetColorBuffer(), D3D12_RESOURCE_STATE_PRESENT);
    commandContext_.Close();
    CommandQueue& commandQueue = graphics_->GetCommandQueue();
    
    commandQueue.Excute(commandContext_);
    swapChain_.Present();

    commandQueue.Signal();
    commandQueue.WaitForGPU();
    commandQueue.UpdateFixFPS();

    commandContext_.Reset();
}

void Renderer::Shutdown() {
    graphics_->Shutdown();
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Finalize();
}


