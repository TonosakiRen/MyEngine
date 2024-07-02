#include "AppendBuffer.h"

#include <cassert>
#include "DirectXCommon.h"
#include "CommandContext.h"

AppendBuffer::~AppendBuffer() {
    Destroy();
}

void AppendBuffer::Create(size_t bufferSize, UINT numElements) {
    // データのサイズ
    UINT size = static_cast<UINT>(bufferSize * numElements);
    UINT countSize = static_cast<UINT>(sizeof(uint32_t));

    UAVBuffer::Create(size + countSize);

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = numElements;
    uavDesc.Buffer.StructureByteStride = UINT(bufferSize);
    uavDesc.Buffer.CounterOffsetInBytes = size;

    uavHandle_ = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    DirectXCommon::GetInstance()->GetDevice()->CreateUnorderedAccessView(resource_.Get(), nullptr, &uavDesc, uavHandle_);
}

void AppendBuffer::Destroy() {
    if (resource_) {
        resource_.Reset();
    }
}
