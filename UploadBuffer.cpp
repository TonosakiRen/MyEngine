#include "UploadBuffer.h"

#include <cassert>

#include "Helper.h"
#include "Graphics.h"

UploadBuffer::~UploadBuffer() {
	Destroy();
}

void UploadBuffer::Create(const std::wstring& name, size_t bufferSize) {
	auto device = Graphics::GetInstance()->GetDevice();
	Destroy();

	auto desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	ASSERT_IF_FAILED(device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(resource_.GetAddressOf())));
	
	state_ = D3D12_RESOURCE_STATE_GENERIC_READ;
	bufferSize_ = bufferSize;
	ASSERT_IF_FAILED(resource_->Map(0, nullptr, &cpuData_));

	D3D12_OBJECT_SET_NAME(resource_, name.c_str());
}

void UploadBuffer::Copy(const void* srcData, size_t copySize) {
	assert(copySize <= bufferSize_);
	memcpy(cpuData_, srcData, copySize);
}

void UploadBuffer::Destroy() {
	if (cpuData_ && resource_) {
		resource_->Unmap(0, nullptr);
		resource_.Reset();
		cpuData_ = nullptr;
	}
}