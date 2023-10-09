#include "LinearAllocator.h"
#include "Graphics.h"
#include "Helper.h"
#include <assert.h>
LinearAllocator::LinearAllocator(size_t pageSize) : pageSize_(pageSize) {

}

LinearAllocator::Allocation LinearAllocator::Allocate(size_t sizeInBytes, size_t alignment) {
	assert(sizeInBytes <= pageSize_);

	if (!currentPage_ || !currentPage_->HasSpace(sizeInBytes,alignment)) {
		currentPage_ = RequestPage();
	}
	return currentPage_->Allocate(sizeInBytes, alignment);
}

std::shared_ptr<LinearAllocator::Page> LinearAllocator::RequestPage() {
	std::shared_ptr<Page> page;
	if (!availablePages_.empty()) {
		page = availablePages_.front();
		availablePages_.pop_front();
	}
	else {
		page = std::make_shared<Page>(pageSize_);
		pagePool_.push_back(page);
	}

	return page;
}

void LinearAllocator::Reset() {
	currentPage_ = nullptr;
	availablePages_ = pagePool_;

	for (auto& page : availablePages_) {
		page->Reset();
	}
}

LinearAllocator::Page::Page(size_t sizeInByte) :
	offset_(0) {
	buffer_.Create(L"LinearAllocator Page", sizeInByte);
}

bool LinearAllocator::Page::HasSpace(size_t sizeinBytes, size_t alignment)const {
	size_t aligmentSize = Helper::AlignUp(sizeinBytes, alignment);
	size_t alignedOffset = Helper::AlignUp(offset_, alignment);
	return alignedOffset + aligmentSize <= buffer_.GetBufferSize();
}

LinearAllocator::Allocation LinearAllocator::Page::Allocate(size_t sizeInBytes, size_t alignment) {
	if (!HasSpace(sizeInBytes, alignment)) {
		throw std::bad_alloc();
	}

	size_t alignedSize = Helper::AlignUp(sizeInBytes, alignment);
	offset_ = Helper::AlignUp(offset_, alignment);

	Allocation allocation;
	allocation.cpu = static_cast<uint8_t*>(buffer_.GetCPUData()) + offset_;
	allocation.gpu = buffer_.GetGPUVirtualAddress() + offset_;

	offset_ += alignedSize;
	return allocation;

}

void LinearAllocator::Page::Reset() {
	offset_ = 0;
}