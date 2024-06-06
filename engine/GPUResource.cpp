#include "GPUResource.h"

#include "DirectXCommon.h"
#include <assert.h>


void GPUResource::CreateResource(
    const D3D12_HEAP_PROPERTIES& heapProperties,
    const D3D12_RESOURCE_DESC& desc,
    D3D12_RESOURCE_STATES initState,
    const D3D12_CLEAR_VALUE* optimizedClearValue) {

    auto graphics = DirectXCommon::GetInstance();
    auto device = graphics->GetDevice();
    auto hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        initState,
        optimizedClearValue,
        IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf()));

    assert(SUCCEEDED(hr));
    state_ = initState;
}