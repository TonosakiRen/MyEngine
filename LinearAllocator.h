#pragma once

#include <memory>
#include <queue>

#include "UploadBuffer.h"
class LinearAllocator
{
public:
	struct Allocation {
		void* cpu;
		D3D12_GPU_VIRTUAL_ADDRESS gpu;
	};

	size_t GetPageSize() const {
		return  pageSize_;
	}

	explicit LinearAllocator(size_t pageSize = 1024 * 1024 * 2); //2MB
	Allocation Allocate(size_t sizeInBytes, size_t alignment = 256);
	void Reset();
private:
	struct Page {
		Page(size_t sizeInBytes);
		bool HasSpace(size_t sizeinBytes, size_t alignment) const;
		Allocation Allocate(size_t sizeInBytes,size_t aligment);
		void Reset();
	private:
		UploadBuffer buffer_;
		size_t offset_;
	};

	std::shared_ptr<Page> RequestPage();

	size_t pageSize_ = 0;
	using PagePool = std::deque<std::shared_ptr<Page>>;
	PagePool pagePool_;
	PagePool availablePages_;
	std::shared_ptr<Page> currentPage_;

};

