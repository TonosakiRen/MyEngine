#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <string>

class GPUResource {
    friend class DirectXCommon;
    friend class CommandContext;
public:
    operator ID3D12Resource* () const { return resource_.Get(); }

    ID3D12Resource* operator->() { return resource_.Get(); }
    const ID3D12Resource* operator->() const { resource_.Get(); }

    ID3D12Resource** GetAddressOf() { return resource_.GetAddressOf(); }

    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return resource_->GetGPUVirtualAddress(); }

    void CreateResource(
        const std::wstring& name,
        const D3D12_HEAP_PROPERTIES& heapProperties,
        const D3D12_RESOURCE_DESC& desc,

        D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_COMMON,
        const D3D12_CLEAR_VALUE* optimizedClearValue = nullptr);

protected:
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    D3D12_RESOURCE_STATES state_ = D3D12_RESOURCE_STATE_COMMON;

#ifdef _DEBUG
    std::wstring name_;
#endif // _DEBUG
};