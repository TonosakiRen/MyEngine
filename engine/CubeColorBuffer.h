#pragma once
#include "PixelBuffer.h"

#include "DescriptorHandle.h"

class CubeColorBuffer : public PixelBuffer {
public:
    void Create(const std::wstring& name, uint32_t width, uint32_t height, DXGI_FORMAT format);

    void SetClearColor(const float* clearColor);
    const float* GetClearColor() const { return clearColor_; }

    const DescriptorHandle& GetSRV() const { return srvHandle_; }
    const DescriptorHandle& GetRTV(uint32_t index) const { return rtvHandle_[index]; }

private:
    void CreateViews();

    float clearColor_[4]{ 0.0f,0.0f,0.0f,0.0f };
    DescriptorHandle srvHandle_;
    DescriptorHandle rtvHandle_[6];
};
