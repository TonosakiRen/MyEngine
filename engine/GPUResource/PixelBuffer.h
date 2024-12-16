#pragma once
/**
 * @file PixelBuffer.h
 * @brief Pixel用Buffer
 */
#include "GPUResource/GPUResource.h"

#include <cstdint>
#include <string>

class PixelBuffer : public GPUResource {
public:
    uint32_t GetWidth() const { return width_; }
    uint32_t GetHeight() const { return height_; }
    uint32_t GetArraySize() const { return arraySize_; }
    DXGI_FORMAT GetFormat() const { return format_; }

protected:
    //DefaultHeapに作るTexutreResource用Create
    void CreateTextureResource(const std::wstring& name, const D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE clearValue);
    //Resourceを他から代入
    void AssociateWithResource(const std::wstring& name, ID3D12Resource* resource, D3D12_RESOURCE_STATES state);
    //Resourceの設定を代入、また設定を返す
    D3D12_RESOURCE_DESC DescribeTex2D(uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags);

    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint32_t arraySize_ = 0;
    DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
};
