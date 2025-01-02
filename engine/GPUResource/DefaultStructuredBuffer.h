#pragma once
/**
 * @file DefaultStructuredBuffer.h
 * @brief DefaultHeap用StructuredBuffer
 */
#include "GPUResource/GPUResource.h"
#include "GPUResource/DefaultBuffer.h"
#include "Graphics/DescriptorHandle.h"

class DefaultStructuredBuffer : public DefaultBuffer {
public:

    void Create(const std::wstring& name, size_t bufferSize, UINT numElements);

    //Getter
    const DescriptorHandle& GetSRV() const {
        return srvHandle_;
    }

protected:

    DescriptorHandle srvHandle_;
};