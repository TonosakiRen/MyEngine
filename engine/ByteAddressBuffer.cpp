#include "ByteAddressBuffer.h"
#include <assert.h>
#include "DirectXCommon.h"
#include "BufferManager.h"

void ByteAddressBuffer::Create(const std::wstring& name, size_t bufferSize , UINT numElements)
{

    HRESULT result = S_FALSE;

    //インスタンシングデータのサイズ
    UINT sizeBAB = static_cast<UINT>(bufferSize * numElements);

    copyBuffer_.Create(name,sizeBAB,&cpuData_);


    auto desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(sizeBAB));
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    resource_ = BufferManager::GetInstance()->CreateResource(
        index_,
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COMMON);

 

    state_ = D3D12_RESOURCE_STATE_COMMON;
    bufferSize_ = sizeBAB;


    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = sizeBAB / 4;
    srvDesc.Buffer.StructureByteStride = 0;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

    srvHandle_ = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);
}

void ByteAddressBuffer::Copy(const void* srcData, size_t copySize, CommandContext& commandContext) {
    assert(copySize <= bufferSize_);
    memcpy(cpuData_, srcData, copySize);
    commandContext.CopyBuffer(*this, copyBuffer_);
    commandContext.TransitionResource(*this, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
}

void ByteAddressBuffer::SetZero()
{
    std::memset(cpuData_, 0, bufferSize_);

}
void ByteAddressBuffer::DestroyCopyBuffer()
{
    BufferManager::GetInstance()->ReleaseResource(copyBuffer_.GetIndex());
}

