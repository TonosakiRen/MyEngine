#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>

#include "DescriptorHandle.h"
#include "DescriptorHeap.h"
#include "CommandQueue.h"
class Graphics {
public:
	static Graphics* GetInstance();
	void Initialize();
	void Shutdown();

	DescriptorHandle AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);

	ID3D12Device* GetDevice() const { return device_.Get(); }
	CommandQueue& GetCommandQueue() { return commandQueue_; }
	DescriptorHeap& GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) { return descriptorHeaps_[type]; }
private:
	static const uint32_t kNumRTVs = 16;
	static const uint32_t kNumDSVs = 2;
	static const uint32_t kNumSRVs = 256;
	static const uint32_t kNumSamplers = 16;

	void CreateDevice();

	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	CommandQueue commandQueue_;
	DescriptorHeap descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
private:
	// シングルトン
	Graphics() = default;
	~Graphics() = default;
	Graphics(const Graphics&) = delete;
	const Graphics& operator=(const Graphics&) = delete;
};