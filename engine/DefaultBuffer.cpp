#include "DefaultBuffer.h"

#include <cassert>
#include "DirectXCommon.h"

DefaultBuffer::~DefaultBuffer() {
    Destroy();
}

void DefaultBuffer::Create(size_t bufferSize) {
    HRESULT result = S_FALSE;
    auto device = DirectXCommon::GetInstance()->GetDevice();

    Destroy();

    auto desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(bufferSize));
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    result = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(resource_.GetAddressOf()));

    assert(SUCCEEDED(result));

#ifdef _DEBUG
    resource_->SetName(L"DefaultBuffer");
#endif // _DEBUG

    state_ = D3D12_RESOURCE_STATE_COMMON;
    bufferSize_ = bufferSize;
}

void DefaultBuffer::Create(size_t numElements, size_t elementSize) {
    Create(numElements * elementSize);
}

void DefaultBuffer::Destroy() {
     resource_.Reset();
}
