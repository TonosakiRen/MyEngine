#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <array>
#include <vector>
#include <utility>
#include "Framework.h"
#include <utility>
class BufferManager{
public:

	static const uint32_t kBufferNum = 100000;

	static BufferManager* GetInstance();
	void Finalize();
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateResource(uint32_t& index, const D3D12_HEAP_PROPERTIES* pHeapProperties,
		D3D12_HEAP_FLAGS HeapFlags,
		const D3D12_RESOURCE_DESC* pDesc,
		D3D12_RESOURCE_STATES InitialResourceState,
		const D3D12_CLEAR_VALUE* optimizedClearValue = nullptr);

	uint32_t AllocateResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource) {

		uint32_t index = 0;

		Microsoft::WRL::ComPtr<ID3D12Resource> comPtrResource = resource;

		for (auto& buffers : buffers_) {
			if (!buffers.second) {
				buffers.second = true;
				buffers.first = comPtrResource;
				break;
			}
			index++;
		}

		return index;
	}

	void ReleaseResource(uint32_t index, bool isMap = false);
	void ReleaseAllResource();

private:
	std::array<std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>,bool>, kBufferNum> buffers_;
	std::vector<std::pair<uint32_t,bool>> releaseIndexes_[2];
private:
	BufferManager() = default;
	~BufferManager() = default;
	BufferManager(const BufferManager&) = delete;
	BufferManager& operator=(const BufferManager&) = delete;
    
};