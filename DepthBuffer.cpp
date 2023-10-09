#include "DepthBuffer.h"
#include "Helper.h"
#include "Graphics.h"
void DepthBuffer::Create(const std::wstring& name, uint32_t width, uint32_t height, DXGI_FORMAT format) {

	D3D12_RESOURCE_DESC desc = DescribeTex2D(width, height, 1, format, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;
	clearValue.DepthStencil.Depth = clearValue_;

    CreateTextureResource(name, desc, clearValue);
    CreateViews();
}


void DepthBuffer::CreateViews() {

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = Helper::GetDSVFormat(format_);
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = Helper::GetDepthFormat(format_);
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;


    Graphics* graphics = Graphics::GetInstance();
    if (dsvHandle_.IsNull()) {
        dsvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    }
    if (srvHandle_.IsNull()) {
        srvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    auto device = graphics->GetDevice();
    device->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsvHandle_);
    device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);
}