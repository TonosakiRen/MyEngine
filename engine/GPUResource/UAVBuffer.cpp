/**
 * @file UAVBuffer.cpp
 * @brief UAVBuffer
 */
#include "GPUResource/UAVBuffer.h"

#include <cassert>
#include "GPUResource/BufferManager.h"
#include <d3dx12.h>
#include "Graphics/Helper.h"

void UAVBuffer::Create(const std::wstring& name, size_t bufferSize) {
    bufferSize = Helper::AlignUp(bufferSize, 256);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(bufferSize), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    resource_ = BufferManager::GetInstance()->CreateResource(
        index_,
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COMMON);


    resource_->SetName(name.c_str());
    name_ = name;


    state_ = D3D12_RESOURCE_STATE_COMMON;
    bufferSize_ = bufferSize;
}

void UAVBuffer::Create(const std::wstring& name, size_t numElements, size_t elementSize) {
    Create(name, numElements * elementSize);
}

