#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>

#include "Graphics/DescriptorHandle.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/CommandQueue.h"

class BufferManager;

class DirectXCommon
{
public:

	//うんち設計
	uint32_t kSrvHeapDescriptorNum = 1024;
	uint32_t kRtvHeapDescriptorNum = 64;
	uint32_t kDsvHeapDescriptorNum = 16;

	//mainColorBufferNum
	uint32_t kMainColorBufferNum = 1;

	static DirectXCommon* GetInstance();
	void Initialize();
	void Shutdown();

	DescriptorHandle AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);

	ID3D12Device5* GetDevice() { return device_.Get(); }

	CommandQueue& GetCommandQueue() { return *commandQueue_.get(); }

	DescriptorHeap& GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) { return *descriptorHeaps_[type]; }

private:

	Microsoft::WRL::ComPtr<ID3D12Device5> device_;
	std::unique_ptr<CommandQueue> commandQueue_;

	std::unique_ptr <DescriptorHeap> descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

private:
	void CreateDevice();
};

