#include "GPUResource.h"
#include "BufferManager.h"
#include <assert.h>

GPUResource::~GPUResource()
{
    if (resource_) {
         BufferManager::GetInstance()->ReleaseResource(index_);
         resource_ = nullptr;
    }
}

void GPUResource::CreateResource(
    const std::wstring& name,
    const D3D12_HEAP_PROPERTIES& heapProperties,
    const D3D12_RESOURCE_DESC& desc,
    D3D12_RESOURCE_STATES initState,
    const D3D12_CLEAR_VALUE* optimizedClearValue) {

    
    resource_ = BufferManager::GetInstance()->CreateResource(
        index_,
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        initState);

    state_ = initState;

#ifdef _DEBUG
    resource_->SetName(name.c_str());
    name_ = name;
#endif // _DEBUG
}
