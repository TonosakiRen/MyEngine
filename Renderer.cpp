//#include "Renderer.h"
//
//#include "ShaderManager.h"
//#include "ImGuiManager.h"
//#include "Helper.h"
//
//Renderer* Renderer::GetInstance() {
//    static Renderer instance;
//    return &instance;
//}
//
//void Renderer::Initialize() {
//    // 描画デバイスを初期化
//    graphics_ = DirectXCommon::GetInstance();
//    graphics_->Initialize();
//
//    // シェーダー関連を初期化
//    ShaderManager::GetInstance()->Initialize();
//
//    // スワップチェーンを初期化
//    auto window = GameWindow::GetInstance();
//    swapChain_.Create(window->GetHWND());
//
//    // コマンドリストを初期化
//    for (auto& commandContext : commandContexts_) {
//        commandContext.Create();
//        commandContext.Close();
//    }
//
//    // メインとなるバッファを初期化
//    auto& swapChainBuffer = swapChain_.GetColorBuffer();
//    float clearColor[4] = { 0.3f,0.1f,0.3f,0.0f };
//    mainColorBuffer_.SetClearColor(clearColor);
//    mainColorBuffer_.Create(L"SceneColorBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT);
//
//    mainDepthBuffer_.Create(L"SceneDepthBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_D32_FLOAT);
//
//    // ブルームを初期化
//    bloom.Initialize(&mainColorBuffer_);
//
//    // ポストエフェクトを初期化
//    InitializePostEffect();
//
//    // ImGuiを初期化
//    auto imguiManager = ImGuiManager::GetInstance();
//    imguiManager->Initialize(window->GetHWND(), swapChain_.GetColorBuffer().GetFormat());
//}
//
//void Renderer::Reset() {
//    auto imguiManager = ImGuiManager::GetInstance();
//    imguiManager->NewFrame();
//
//    auto& commandContext = commandContexts_[swapChain_.GetBufferIndex()];
//
//    commandContext.Reset();
//}
//
//void Renderer::BeginRender() {
//    auto& commandContext = commandContexts_[swapChain_.GetBufferIndex()];
//
//    // メインカラーバッファをレンダ―ターゲットに
//    commandContext.TransitionResource(mainColorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
//    commandContext.TransitionResource(mainDepthBuffer_, D3D12_RESOURCE_STATE_DEPTH_WRITE);
//    commandContext.SetRenderTarget(mainColorBuffer_.GetRTV(), mainDepthBuffer_.GetDSV());
//    commandContext.ClearColor(mainColorBuffer_);
//    commandContext.ClearDepth(mainDepthBuffer_);
//    commandContext.SetViewportAndScissorRect(0, 0, mainColorBuffer_.GetWidth(), mainColorBuffer_.GetHeight());
//}
//
//void Renderer::EndRender() {
//    auto& commandContext = commandContexts_[swapChain_.GetBufferIndex()];
//
//    // メインカラーバッファにブルームをかける
//
//    bloom.Render(commandContext);
//
//    // スワップチェーンをレンダ―ターゲットに
//    auto& swapChainBuffer = swapChain_.GetColorBuffer();
//    commandContext.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
//    commandContext.SetRenderTarget(swapChainBuffer.GetRTV());
//    commandContext.ClearColor(swapChainBuffer);
//    commandContext.SetViewportAndScissorRect(0, 0, swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());
//
//    //ColorBuffer& buffer = bloom.GetLuminanceTexture();
//    ColorBuffer& buffer = mainColorBuffer_;
//    // メインカラーバッファをスワップチェーンに移す
//    commandContext.TransitionResource(buffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
//    commandContext.SetRootSignature(postEffectRootSignature_);
//    commandContext.SetPipelineState(postEffectPipelineState_);
//    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//    commandContext.SetDescriptorTable(0, buffer.GetSRV());
//    commandContext.Draw(3);
//
//    // ImGuiを描画
//    auto imguiManager = ImGuiManager::GetInstance();
//    imguiManager->Render(commandContext);
//
//    commandContext.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_PRESENT);
//    commandContext.Close();
//    CommandQueue& commandQueue = graphics_->GetCommandQueue();
//    commandQueue.WaitForGPU();
//    commandQueue.Excute(commandContext);
//    swapChain_.Present();
//    commandQueue.Signal();
//}
//
//void Renderer::Shutdown() {
//    graphics_->Shutdown();
//    auto imguiManager = ImGuiManager::GetInstance();
//    imguiManager->Shutdown();
//}
//
//void Renderer::InitializePostEffect() {
//    {
//        CD3DX12_DESCRIPTOR_RANGE ranges[1]{};
//        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
//
//        CD3DX12_ROOT_PARAMETER rootParameters[1]{};
//        rootParameters[0].InitAsDescriptorTable(_countof(ranges), ranges);
//
//        CD3DX12_STATIC_SAMPLER_DESC staticSampler(
//            0,
//            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
//            D3D12_TEXTURE_ADDRESS_MODE_BORDER,
//            D3D12_TEXTURE_ADDRESS_MODE_BORDER,
//            D3D12_TEXTURE_ADDRESS_MODE_BORDER,
//            D3D12_DEFAULT_MIP_LOD_BIAS, D3D12_DEFAULT_MAX_ANISOTROPY,
//            D3D12_COMPARISON_FUNC_LESS_EQUAL,
//            D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE);
//
//        D3D12_ROOT_SIGNATURE_DESC desc{};
//        desc.pParameters = rootParameters;
//        desc.NumParameters = _countof(rootParameters);
//        desc.pStaticSamplers = &staticSampler;
//        desc.NumStaticSamplers = 1;
//        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//
//        postEffectRootSignature_.Create(L"PostEffect RootSignature", desc);
//    }
//
//    {
//        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
//        DXGI_FORMAT rtvFormat = swapChain_.GetColorBuffer().GetFormat();
//
//        auto shaderManager = ShaderManager::GetInstance();
//
//        desc.pRootSignature = postEffectRootSignature_;
//
//        auto vs = shaderManager->Compile(L"Resources/Shader/ScreenQuadVS.hlsl", ShaderManager::kVertex);
//        auto ps = shaderManager->Compile(L"Resources/Shader/PostEffectPS.hlsl", ShaderManager::kPixel);
//        desc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
//        desc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());
//        desc.BlendState = Helper::BlendDisable;
//        desc.RasterizerState = Helper::RasterizerNoCull;
//        desc.NumRenderTargets = 1;
//        desc.RTVFormats[0] = rtvFormat;
//        desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
//        desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//        desc.SampleDesc.Count = 1;
//        postEffectPipelineState_.Create(L"PostEffect PSO", desc);
//    }
//}
