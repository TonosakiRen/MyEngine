/**
 * @file GPUResource.cpp
 * @brief GPUResource
 */
#include "GPUResource/GPUResource.h"
#include "GPUResource/BufferManager.h"
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
    D3D12_RESOURCE_STATES initState) {

    
    resource_ = BufferManager::GetInstance()->CreateResource(
        index_,
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        initState);

    state_ = initState;


    resource_->SetName(name.c_str());
    name_ = name;
}
