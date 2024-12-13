/**
 * @file DefaultStructuredBuffer.h
 * @brief DefaultHeap用StructuredBuffer
 */
#include "GPUResource/DefaultStructuredBuffer.h"

#include <cassert>
#include "Graphics/DirectXCommon.h"
#include "Graphics/Helper.h"

void DefaultStructuredBuffer::Create(const std::wstring& name, size_t bufferSize, UINT numElements) {
    // インスタンシングデータのサイズ
    UINT sizeINB = static_cast<UINT>(bufferSize * numElements);
    sizeINB = Helper::AlignUp(sizeINB, 256);
    DefaultBuffer::Create(name,sizeINB);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = numElements;
    srvDesc.Buffer.StructureByteStride = UINT(bufferSize);

    srvHandle_ = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);

    copyBuffer_.Create(name, sizeINB);

}


