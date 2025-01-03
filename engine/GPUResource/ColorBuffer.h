#pragma once
/**
 * @file ColorBuffer.h
 * @brief 一枚絵用Buffer
 */
#include "GPUResource/PixelBuffer.h"

#include "Graphics/DescriptorHandle.h"

class ColorBuffer : public PixelBuffer {
public:
    void Create(const std::wstring& name, uint32_t width, uint32_t height, DXGI_FORMAT format);
    void CreateArray(const std::wstring& name, uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format);
    //SwapChain用Create関数
    void CreateFromSwapChain(const std::wstring& name, ID3D12Resource* resource, uint32_t index);

    //Setter
    void SetClearColor(const float* clearColor);
    //Getter
    const float* GetClearColor() const { return clearColor_; }
    const DescriptorHandle& GetSRV() const { return srvHandle_; }
    const DescriptorHandle& GetRTV() const { return rtvHandle_; }

private:
    //Viewを生成
    void CreateViews();

    float clearColor_[4]{ 0.0f,0.0f,0.0f,0.0f };
    DescriptorHandle srvHandle_;
    DescriptorHandle rtvHandle_;
};
