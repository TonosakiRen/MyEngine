/**
 * @file StructuredBuffer.cpp
 * @brief StructuredBuffer
 */
#include "GPUResource/StructuredBuffer.h"
#include "GPUResource/BufferManager.h"
#include "Graphics/DirectXCommon.h"
#include <assert.h>
#include "Graphics/Helper.h"

void StructuredBuffer::Create(const std::wstring& name, size_t bufferSize , UINT numElements)
{
    // インスタンシングデータのサイズ


    UINT sizeINB = static_cast<UINT>(bufferSize * numElements);

    HRESULT result = S_FALSE;


    auto desc = CD3DX12_RESOURCE_DESC::Buffer(Helper::AlignUp(sizeINB * 2, 256));
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
    bufferSize_ = sizeINB;

    result = resource_->Map(0, nullptr, &cpuData_);
    assert(SUCCEEDED(result));


    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = numElements;
    srvDesc.Buffer.StructureByteStride = UINT(bufferSize);


    srvHandle_[0] = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_[0]);

    srvDesc.Buffer.FirstElement = numElements;

    srvHandle_[1] = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_[1]);
}
