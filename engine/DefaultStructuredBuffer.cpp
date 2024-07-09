#include "DefaultStructuredBuffer.h"

#include <cassert>
#include "DirectXCommon.h"
#include "CommandContext.h"

DefaultStructuredBuffer::~DefaultStructuredBuffer() {
    uploadBuffer_.~UploadBuffer();
    Destroy();
}

void DefaultStructuredBuffer::Create(const std::wstring& name, size_t bufferSize, UINT numElements) {
    // インスタンシングデータのサイズ
    UINT sizeINB = static_cast<UINT>(bufferSize * numElements);

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

    uploadBuffer_.Create(name, sizeINB);

}

void DefaultStructuredBuffer::Copy(const void* srcData, size_t copySize) {
    uploadBuffer_.Copy(srcData, copySize);
    isCopy_ = true;
}

void DefaultStructuredBuffer::BufferCopy(CommandContext& commandContext)
{
    commandContext.CopyBuffer(*this, uploadBuffer_);
    commandContext.TransitionResource(*this, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
}

void DefaultStructuredBuffer::Destroy() {
    if (resource_) {
        resource_.Reset();
    }
}
