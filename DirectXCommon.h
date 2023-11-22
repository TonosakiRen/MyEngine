#pragma once
#include <Windows.h>
#include <cstdlib>
#include <d3d12.h>
#include "externals/DirectXTex/d3dx12.h"
#include "WinApp.h"
#include "DescriptorHeap.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "PostEffect.h"
#include "CommandContext.h"
#include "GaussianBlur.h"
#include "Bloom.h"
#include "ShaderManager.h"

class DirectXCommon
{
public:

	//うんち設計
	uint32_t kSrvHeapDescritorNum = 1024;
	uint32_t kRtvHeapDescritorNum = 32;

	//mainColorBufferNum
	uint32_t kMainColorBufferNum = 1;

	static DirectXCommon* GetInstance();
	void Initialize(int32_t backBufferWidth = 1280, int32_t backBufferHeight = 720);
	void InitializePostEffect();
	
	void MainPreDraw();
	void MainPostDraw();
	void SwapChainPreDraw();
	void SwapChainPostDraw();

	DescriptorHandle AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);

	ID3D12Device* GetDevice() { return device_.Get(); }
	CommandQueue& GetCommandQueue() { return commandQueue_; }
	CommandContext* GetCommandContext() { return &commandContext_; }

	DescriptorHeap& GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) { return descriptorHeaps_[type]; }

	void ClearMainDepthBuffer() { commandContext_.ClearDepth(mainDepthBuffer_); }

private:
	WinApp* winApp_;

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	CommandQueue commandQueue_;
	CommandContext commandContext_;
	SwapChain swapChain_;

	DepthBuffer mainDepthBuffer_;
	ColorBuffer mainColorBuffer_;

	PostEffect postEffect_;
	Bloom bloom_;

	DescriptorHeap descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;

private:
	void InitializeDXGIDevice();
};

