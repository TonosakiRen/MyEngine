#include "GPUResource/UploadBuffer.h"

#include <cassert>
#include "GPUResource/BufferManager.h"

UploadBuffer::~UploadBuffer()
{
   
}

void UploadBuffer::Create(const std::wstring& name, size_t bufferSize) {
    HRESULT result = S_FALSE;
   

    bufferSize = Helper::AlignUp(bufferSize, 256);

    auto desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize * 2);
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    resource_ = BufferManager::GetInstance()->CreateResource(
        index_,
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ
        );

#ifdef _DEBUG
    resource_->SetName(name.c_str());
    name_ = name;
#endif // _DEBUG


    state_ = D3D12_RESOURCE_STATE_GENERIC_READ;
    bufferSize_ = bufferSize;

    result =  resource_->Map(0, nullptr, &cpuData_);
    assert(SUCCEEDED(result));
}

void UploadBuffer::Create(const std::wstring& name, size_t bufferSize, bool a)
{
    HRESULT result = S_FALSE;


    auto desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize * 2);
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    resource_ = BufferManager::GetInstance()->CreateResource(
        index_,
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ
    );

#ifdef _DEBUG
    resource_->SetName(name.c_str());
    name_ = name;
#endif // _DEBUG


    state_ = D3D12_RESOURCE_STATE_GENERIC_READ;
    bufferSize_ = bufferSize;

    result = resource_->Map(0, nullptr, &cpuData_);
    assert(SUCCEEDED(result));
}



void UploadBuffer::Copy(const void* srcData, size_t copySize) {
    assert(copySize <= bufferSize_);
    if (!isFirstUpload_) {
        memcpy(static_cast<char*>(cpuData_) + (Framework::kFrameRemainder * bufferSize_), srcData, copySize);
    }
    else {
        isFirstUpload_ = false;
        memcpy(static_cast<char*>(cpuData_) , srcData, copySize);
        memcpy(static_cast<char*>(cpuData_) + bufferSize_, srcData, copySize);
    }
}

void UploadBuffer::SetZero()
{
    std::memset(cpuData_, 0, bufferSize_ );
}
