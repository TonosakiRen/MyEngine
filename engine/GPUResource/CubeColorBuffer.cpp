/**
 * @file CubeColorBuffer.cpp
 * @brief CubeTexture用Buffer
 */
#include "GPUResource/CubeColorBuffer.h"

#include "Graphics/DirectXCommon.h"

void CubeColorBuffer::Create(const std::wstring& name, uint32_t width, uint32_t height, DXGI_FORMAT format) {
    auto flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    auto desc = DescribeTex2D(width, height, 6, format, flags);

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = format;
    memcpy(clearValue.Color, clearColor_, sizeof(clearValue.Color));

    CreateTextureResource(name,desc, clearValue);
    CreateViews();
}

void CubeColorBuffer::SetClearColor(const float* clearColor) {
    memcpy(clearColor_, clearColor, sizeof(clearColor_));
}

void CubeColorBuffer::CreateViews() {
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc[6]{};
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    for (int i = 0; i < 6; i++) {
        rtvDesc[i].Format = format_;
    }
    srvDesc.Format = format_;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = UINT_MAX;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;

    for (int i = 0; i < 6; i++) {
        rtvDesc[i].ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
        rtvDesc[i].Texture2DArray.MipSlice = 0;
        rtvDesc[i].Texture2DArray.FirstArraySlice = i;
        rtvDesc[i].Texture2DArray.PlaneSlice = 0;
        rtvDesc[i].Texture2DArray.ArraySize = 1;
    }

    auto graphics = DirectXCommon::GetInstance();
    for (int i = 0; i < 6; i++) {
        if (rtvHandle_[i].IsNull()) {
            rtvHandle_[i] = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }
    }
    if (srvHandle_.IsNull()) {
        srvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    auto device = graphics->GetDevice();
    for (int i = 0; i < 6; i++) {
        device->CreateRenderTargetView(resource_.Get(), &rtvDesc[i], rtvHandle_[i]);
    }
    device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);
}
