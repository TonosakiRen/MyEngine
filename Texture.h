#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <string>

#include "GPUResource.h"

#include "DescriptorHandle.h"

class Texture : public GPUResource {
public:
    void CreateFromWICFile(const std::wstring& path);

    const D3D12_RESOURCE_DESC& GetDesc() const { return desc_; }
    const DescriptorHandle& GetSRV() const { return srvHandle_; }

private:
    void CreateView();

    D3D12_RESOURCE_DESC desc_{};
    DescriptorHandle srvHandle_;
};