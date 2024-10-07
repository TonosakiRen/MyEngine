#include "RwStructuredBuffer.h"

#include <cassert>
#include "DirectXCommon.h"
#include "CommandContext.h"

void RwStructuredBuffer::Create(const std::wstring& name, size_t bufferSize, UINT numElements) {
    // インスタンシングデータのサイズ
    UINT sizeINB = static_cast<UINT>(bufferSize * numElements);

    UAVBuffer::Create(name, sizeINB);

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = numElements;
    uavDesc.Buffer.StructureByteStride = UINT(bufferSize);
    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

    uavHandle_ = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    DirectXCommon::GetInstance()->GetDevice()->CreateUnorderedAccessView(resource_.Get(), nullptr, &uavDesc, uavHandle_);
}


