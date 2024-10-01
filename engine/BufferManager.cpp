#include "BufferManager.h"
#include <algorithm>
#include <assert.h>
#include "DirectXCommon.h"

BufferManager* BufferManager::GetInstance()
{
	static BufferManager instance;
	return &instance;
}

void BufferManager::Finalize()
{
	for (auto& buffer : buffers_) {
		if (buffer.first) {
			buffer.first.Reset();
		}
	}
}

Microsoft::WRL::ComPtr<ID3D12Resource> BufferManager::CreateResource(uint32_t& index,const D3D12_HEAP_PROPERTIES* pHeapProperties, D3D12_HEAP_FLAGS HeapFlags, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES InitialResourceState, const D3D12_CLEAR_VALUE* optimizedClearValue)
{

	Microsoft::WRL::ComPtr<ID3D12Resource> comPtrResource;

	HRESULT result = DirectXCommon::GetInstance()->DirectXCommon::GetDevice()->CreateCommittedResource(
		pHeapProperties,
		HeapFlags,
		pDesc,
		InitialResourceState,
		optimizedClearValue,
		IID_PPV_ARGS(&comPtrResource));

	assert(SUCCEEDED(result));

	uint32_t index_ = 0;

	for (auto& buffers : buffers_) {
		if (!buffers.second) {
			buffers.second = true;
			buffers.first = comPtrResource;
			break;
		}
		index_++;
	}
	
	index = index_;

	return comPtrResource;
}

void BufferManager::ReleaseResource(uint32_t index , bool isMap)
{
	releaseIndexes_[Framework::kFrameRemainder].emplace_back(index, isMap);
}

void BufferManager::ReleaseAllResource()
{

    for (auto& index : releaseIndexes_[!Framework::kFrameRemainder]) {
		if (index.second == true) {
			buffers_[index.first].first->Unmap(0,nullptr);
		}
		buffers_[index.first].first.Reset();
		buffers_[index.first].second = false;
    }

    releaseIndexes_[!Framework::kFrameRemainder].clear();
}
