#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>

#include "DescriptorHandle.h"

class DescriptorHeap
{
public:

	void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDesctiptors);
	DescriptorHandle Allocate();
	void Reset(uint32_t reallocationOffset = 0);
	operator ID3D12DescriptorHeap* () const { return descriptorHeap_.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	D3D12_DESCRIPTOR_HEAP_TYPE type_ = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
	DescriptorHandle descriptorStart_;
	uint32_t numDescriptors_ = 0;
	uint32_t descriptorSize_ = 0;
	uint32_t freeDescriptorOffset_ = 0;
};

