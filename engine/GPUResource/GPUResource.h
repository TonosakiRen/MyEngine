#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <string>

class GPUResource {
    friend class DirectXCommon;
    friend class CommandContext;
    friend class ReleaseManager;
public:
    ~GPUResource();

    operator ID3D12Resource* () const { return resource_.Get(); }

    ID3D12Resource* operator->() { return resource_.Get(); }
    const ID3D12Resource* operator->() const { resource_; }

    ID3D12Resource** GetAddressOf() { return &resource_; }

    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return resource_->GetGPUVirtualAddress(); }

    void CreateResource(
        const std::wstring& name,
        const D3D12_HEAP_PROPERTIES& heapProperties,
        const D3D12_RESOURCE_DESC& desc,
        D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_COMMON);

    void SetName(const std::wstring& name) {
        resource_->SetName(name.c_str());
        name_ = name;
    }
    void SetState(const D3D12_RESOURCE_STATES state) {
        state_ = state;
    }

    HRESULT Map(void* cpuData) {
        return(resource_->Map(0, nullptr, &cpuData));
    }

    uint32_t GetIndex() {
        return index_;
    }

  

protected:
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;
    D3D12_RESOURCE_STATES state_ = D3D12_RESOURCE_STATE_COMMON;
    uint32_t index_ = 0;
    std::wstring name_;
};