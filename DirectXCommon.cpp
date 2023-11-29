#include "DirectXCommon.h"
#include <assert.h>
#include "Helper.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

using namespace Microsoft::WRL;

DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::Initialize(int32_t backBufferWidth, int32_t backBufferHeight) {

	assert(4 <= backBufferWidth && backBufferWidth <= 4096);
	assert(4 <= backBufferHeight && backBufferHeight <= 4096);

	winApp_ = WinApp::GetInstance();
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;

	InitializeDXGIDevice();
	commandQueue_.Create();
	commandContext_.Create();

	/*descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_RTV].Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, SwapChain::kNumBuffers + kMainColorBufferNum)*/;
	descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_RTV].Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kRtvHeapDescritorNum);
	descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_DSV].Create(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
	descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kSrvHeapDescritorNum);

	commandContext_.SetDescriptorHeap();

	swapChain_.Create(winApp_->GetHwnd());

	mainColorBuffer_.Create(backBufferWidth_, backBufferHeight_, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

	mainDepthBuffer_.Create(backBufferWidth_, backBufferHeight_, DXGI_FORMAT_D24_UNORM_S8_UINT);
	commandContext_.TransitionResource(mainDepthBuffer_, D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void DirectXCommon::InitializePostEffect()
{
	bloom_.Initialize(&mainColorBuffer_);
	postEffect_.Initialize();

}

void DirectXCommon::MainPreDraw() {

	commandContext_.TransitionResource(mainColorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	DescriptorHandle rtvH = mainColorBuffer_.GetRTV();

	DescriptorHandle dsvH = mainDepthBuffer_.GetDSV();

	commandContext_.SetRenderTarget(mainColorBuffer_.GetRTV(), mainDepthBuffer_.GetDSV());

	// 全画面クリア
	commandContext_.ClearColor(mainColorBuffer_);
	// 深度バッファクリア
	commandContext_.ClearDepth(mainDepthBuffer_);

	// ビューポートの設定
	CD3DX12_VIEWPORT viewport =
		CD3DX12_VIEWPORT(0.0f, 0.0f, float(backBufferWidth_), float(backBufferHeight_));

	commandContext_.SetViewport(viewport);

	// シザリング矩形の設定
	CD3DX12_RECT rect = CD3DX12_RECT(0, 0, backBufferWidth_, backBufferHeight_);
	commandContext_.SetScissorRect(rect);
}

void DirectXCommon::MainPostDraw() {

	bloom_.Render(commandContext_);


	commandContext_.TransitionResource(mainColorBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	commandContext_.TransitionResource(swapChain_.GetColorBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET);

}

void DirectXCommon::SwapChainPreDraw()
{
	// ビューポートの設定
	CD3DX12_VIEWPORT viewport =
		CD3DX12_VIEWPORT(0.0f, 0.0f, float(backBufferWidth_), float(backBufferHeight_));

	commandContext_.SetViewport(viewport);

	// シザリング矩形の設定
	CD3DX12_RECT rect = CD3DX12_RECT(0, 0, backBufferWidth_, backBufferHeight_);
	commandContext_.SetScissorRect(rect);

	// レンダーターゲットをセット
	commandContext_.SetRenderTarget(swapChain_.GetRTV());

	commandContext_.ClearColor(swapChain_.GetColorBuffer());

	PostEffect::PreDraw(commandContext_);
	postEffect_.Draw(mainColorBuffer_.GetSRV());
	PostEffect::PostDraw();
}

void DirectXCommon::SwapChainPostDraw()
{

	commandContext_.TransitionResource(swapChain_.GetColorBuffer(), D3D12_RESOURCE_STATE_PRESENT);

	// 命令のクローズ
	commandContext_.Close();

	commandQueue_.Execute(commandContext_);

	// バッファをフリップ
	swapChain_.Present();

	HRESULT result = FALSE;
#ifdef _DEBUG
	if (FAILED(result)) {
		ComPtr<ID3D12DeviceRemovedExtendedData> dred;

		Helper::AssertIfFailed(device_->QueryInterface(IID_PPV_ARGS(&dred)));

		// 自動パンくず取得
		D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT autoBreadcrumbsOutput{};
		Helper::AssertIfFailed(dred->GetAutoBreadcrumbsOutput(&autoBreadcrumbsOutput));
	}
#endif

	commandQueue_.Signal();
	commandQueue_.WaitForGPU();
	commandQueue_.UpdateFixFPS();

	commandContext_.Reset();
}

void DirectXCommon::InitializeDXGIDevice() {

#ifdef _DEBUG

	ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバックレイヤーを有効化する	
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行えるようにする
		debugController->SetEnableGPUBasedValidation(TRUE);

	}

#endif

	//DXGIファクトリー
	dxgiFactory_ = nullptr;
	//HRESULTはWIndows系のエラーコードであり、
	//関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	Helper::AssertIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_)));
	//初期化の根本的な部分でエラーが出た場合はプログラングが間違っているか、
	//どうにもできない場合が多いのでassertにしておく

	//使用するアダプタ用の変数、最初にnullptrを入れておく
	ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	//良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプターの情報を取得する	
		DXGI_ADAPTER_DESC3 adapterDesc{};
		Helper::AssertIfFailed(useAdapter->GetDesc3(&adapterDesc));
		//ソフトウェアアダプタでなければ採用！
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Helper::Log(std::format(L"Use Adapter{}\n", adapterDesc.Description));
			break;
		}
		useAdapter = nullptr; //ソフトウェアアダプタの場合は見なかったことにする
	}
	//適切なアダプタが見つからんかったので起動できない
	assert(useAdapter != nullptr);

	//D3D12Deviceの生成
	device_ = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	HRESULT hr;
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		//指定した機能レベルでデバイスが生成できたか確認
		if (SUCCEEDED(hr)) {
			//生成できたのでログ出力を行ってループを抜ける	
			Helper::Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	//デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
	Helper::Log("Complete create D3D12Device!!!\n");//初期化完了のログを出す

	const uint32_t desriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t desriptorSizeRTV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t desriptorSizeDSV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);


#ifdef _DEBUG

	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//やばいエラー前に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に泊まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に泊まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバックレイヤーとDX12デバッグレイヤーの相方作用バグによるエラーメッセージ
			//https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif

}

DescriptorHandle DirectXCommon::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type) {
	return descriptorHeaps_[type].Allocate();
}
