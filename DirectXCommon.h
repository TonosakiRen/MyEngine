#pragma once
#include <Windows.h>
#include <cstdlib>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include "externals/DirectXTex/d3dx12.h"
#include "WinApp.h"

class DirectXCommon
{
public:

	static const uint32_t kNumFrameBuffers = 2;

	enum RTVLayout {
		FrameBufferStart,
		ShadowMapBuffer = FrameBufferStart + kNumFrameBuffers,
		NumRTV,
	};

	enum DSVLayout {
		DefaultDepthBuffer,
		ShadowDepthBuffer,
		NumDSV
	};

	static DirectXCommon* GetInstance();
	void Initialize(int32_t backBufferWidth = 1280, int32_t backBufferHeight = 720);
	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// レンダーターゲットのクリア
	/// </summary>
	void ClearRenderTarget();

	/// <summary>
	/// 深度バッファのクリア
	/// </summary>
	void ClearDepthBuffer();

	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		//compilerするShaderファイルへのパス
		const std::wstring& filePath,
		//Compilerに使用するprofile
		const wchar_t* profile);

	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns>デバイス</returns>
	ID3D12Device* GetDevice() { return device_.Get(); }

	/// <summary>
	/// 描画コマンドリストの取得
	/// </summary>
	/// <returns>描画コマンドリスト</returns>
	ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }


	// バックバッファの数を取得
	size_t GetBackBufferCount() const { return backBuffers_.size(); }

private:
	// ウィンドウズアプリケーション管理
	WinApp* winApp_;
	// Direct3D関連
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	UINT64 fenceVal_ = 0;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;

	
	//textureManager
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
	
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;
private:
	// DXGIデバイス初期化
	void InitializeDXGIDevice();
	// コマンド関連初期化
	void InitializeCommand();
	// スワップチェーンの生成
	void CreateSwapChain();
	//DirectXCompiler生成
	void CreateDirectXCompilier();
	// レンダーターゲット生成
	void CreateFinalRenderTargets();
	// 深度バッファ生成
	void CreateDepthBuffer();
	// フェンス生成
	void CreateFence();
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource();
};

