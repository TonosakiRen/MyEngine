#pragma once
#include "GPUResource.h"
#include "DefaultBuffer.h"
#include "DescriptorHandle.h"

class DefaultStructuredBuffer : public DefaultBuffer {
public:

    void Create(const std::wstring& name, size_t bufferSize, UINT numElements);

    const DescriptorHandle& GetSRV() const {
        return srvHandle_;
    }

protected:

    DescriptorHandle srvHandle_;
};