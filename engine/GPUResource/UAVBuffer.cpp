#include "UAVBuffer.h"

#include <cassert>
#include "BufferManager.h"
#include <d3dx12.h>

void UAVBuffer::Create(const std::wstring& name, size_t bufferSize) {
    HRESULT result = S_FALSE;


    auto desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(bufferSize), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    resource_ = BufferManager::GetInstance()->CreateResource(
        index_,
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COMMON);

#ifdef _DEBUG
    resource_->SetName(name.c_str());
    name_ = name;
#endif // _DEBUG

    state_ = D3D12_RESOURCE_STATE_COMMON;
    bufferSize_ = bufferSize;
}

void UAVBuffer::Create(const std::wstring& name, size_t numElements, size_t elementSize) {
    Create(name, numElements * elementSize);
}

