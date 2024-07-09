#include "RwStructuredBuffer.h"

#include <cassert>
#include "DirectXCommon.h"
#include "CommandContext.h"

RwStructuredBuffer::~RwStructuredBuffer() {
    Destroy();
}

void RwStructuredBuffer::Create(const std::wstring& name, size_t bufferSize, UINT numElements) {
    // インスタンシングデータのサイズ
    UINT sizeINB = static_cast<UINT>(bufferSize * numElements);

    UAVBuffer::Create(name, sizeINB);

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.NumElements = numElements;
    uavDesc.Buffer.StructureByteStride = UINT(bufferSize);

    uavHandle_ = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    DirectXCommon::GetInstance()->GetDevice()->CreateUnorderedAccessView(resource_.Get(), nullptr, &uavDesc, uavHandle_);
}

void RwStructuredBuffer::Destroy() {
    if (resource_) {
        resource_.Reset();
    }
}
