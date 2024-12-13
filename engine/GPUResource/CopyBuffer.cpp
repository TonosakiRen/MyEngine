/**
 * @file CopyBuffer.cpp
 * @brief コピー用buffer
 */
#include "GPUResource/CopyBuffer.h"

#include <cassert>
#include "Graphics/DirectXCommon.h"
#include "GPUResource/BufferManager.h"
#include "Graphics/Helper.h"

void CopyBuffer::Create(const std::wstring& name, size_t bufferSize,void** cpuData) {
    HRESULT result = S_FALSE;
    bufferSize = Helper::AlignUp(bufferSize,256);

    auto desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(bufferSize));
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    resource_ = BufferManager::GetInstance()->CreateResource(
        index_,
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ);

    resource_->SetName(name.c_str());
    name_ = name;

    assert(SUCCEEDED(result));

    state_ = D3D12_RESOURCE_STATE_GENERIC_READ;
    bufferSize_ = bufferSize;

    result = resource_->Map(0, nullptr, cpuData);
    assert(SUCCEEDED(result));
}

void CopyBuffer::Create(const std::wstring& name, size_t bufferSize) {
    HRESULT result = S_FALSE;

    auto desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(bufferSize));
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    resource_ = BufferManager::GetInstance()->CreateResource(
        index_,
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ);

   
    resource_->SetName(name.c_str());
    name_ = name;


    state_ = D3D12_RESOURCE_STATE_GENERIC_READ;
    bufferSize_ = bufferSize;

    result = resource_->Map(0, nullptr, &cpuData_);
    assert(SUCCEEDED(result));
}

void CopyBuffer::Copy(const void* srcData, size_t copySize) const
{
    memcpy(cpuData_, srcData, copySize);
}

void CopyBuffer::SetZero()
{
    std::memset(cpuData_, 0, bufferSize_);
}

