#include "GPUResource/DefaultBuffer.h"

#include <cassert>
#include "Graphics/DirectXCommon.h"
#include "GPUResource/BufferManager.h"
#include "Graphics/Helper.h"


void DefaultBuffer::Create(const std::wstring& name, size_t bufferSize) {
    bufferSize = Helper::AlignUp(bufferSize, 256);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(bufferSize));
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

    copyBuffer_.Create(name, bufferSize_);
}

void DefaultBuffer::Create(const std::wstring& name, size_t numElements, size_t elementSize) {
    Create(name, numElements * elementSize);
}

void DefaultBuffer::Copy(const void* srcData, size_t copySize, CommandContext& commandContext)
{
    copyBuffer_.Copy(srcData, copySize);
    commandContext.CopyBuffer(*this, copyBuffer_);
}

void DefaultBuffer::DestroyCopyBuffer()
{
    BufferManager::GetInstance()->ReleaseResource(copyBuffer_.GetIndex());
}
