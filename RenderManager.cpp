#include "RenderManager.h"

#include "ShaderManager.h"
#include "WinApp.h"
#include "ImGuiManager.h"
#include "Helper.h"

RenderManager* RenderManager::GetInstance() {
    static RenderManager instance;
    return &instance;
}

void RenderManager::Initialize() {
    // 描画デバイスを初期化
    graphics_ = Graphics::GetInstance();
    graphics_->Initialize();

    // シェーダー関連を初期化
    ShaderManager::GetInstance()->Initialize();

    // スワップチェーンを初期化
    auto window = WinApp::GetInstance();
    swapChain_.Create(window->GetHwnd());

    // コマンドリストを初期化
    for (auto& commandContext : commandContexts_) {
        commandContext.Create();
        commandContext.Close();
    }

    // メインとなるバッファを初期化
    auto& swapChainBuffer = swapChain_.GetColorBuffer();
    float clearColor[4] = { 0.3f,0.1f,0.3f,0.0f };
    mainColorBuffer_.SetClearColor(clearColor);
    mainColorBuffer_.Create(L"SceneColorBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT);

    mainDepthBuffer_.Create(L"SceneDepthBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_D32_FLOAT);

    // ImGuiを初期化
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Initialize(window->GetHwnd(), swapChain_.GetColorBuffer().GetFormat());
}

void RenderManager::Reset() {
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->NewFrame();

    auto& commandContext = commandContexts_[swapChain_.GetBufferIndex()];

    commandContext.Reset();
}

void RenderManager::BeginRender() {
    auto& commandContext = commandContexts_[swapChain_.GetBufferIndex()];

    // メインカラーバッファをレンダ―ターゲットに
    commandContext.TransitionResource(mainColorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.TransitionResource(mainDepthBuffer_, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    commandContext.SetRenderTarget(mainColorBuffer_.GetRTV(), mainDepthBuffer_.GetDSV());
    commandContext.ClearColor(mainColorBuffer_);
    commandContext.ClearDepth(mainDepthBuffer_);
    commandContext.SetViewportAndScissorRect(0, 0, mainColorBuffer_.GetWidth(), mainColorBuffer_.GetHeight());
}

void RenderManager::EndRender() {
    auto& commandContext = commandContexts_[swapChain_.GetBufferIndex()];

    // スワップチェーンをレンダ―ターゲットに
    auto& swapChainBuffer = swapChain_.GetColorBuffer();
    commandContext.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.SetRenderTarget(swapChainBuffer.GetRTV());
    commandContext.ClearColor(swapChainBuffer);
    commandContext.SetViewportAndScissorRect(0, 0, swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());

    ColorBuffer& buffer = mainColorBuffer_;
    // メインカラーバッファをスワップチェーンに移す
    commandContext.TransitionResource(buffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    /*commandContext.SetRootSignature(postEffectRootSignature_);
    commandContext.SetPipelineState(postEffectPipelineState_);*/
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetDescriptorTable(0, buffer.GetSRV());
    commandContext.Draw(3);

    // ImGuiを描画
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Render(commandContext);

    commandContext.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_PRESENT);
    commandContext.Close();
    CommandQueue& commandQueue = graphics_->GetCommandQueue();
    commandQueue.WaitForGPU();
    commandQueue.Excute(commandContext);
    swapChain_.Present();
    commandQueue.Signal();
}

void RenderManager::Shutdown() {
    graphics_->Shutdown();
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Shutdown();
}
