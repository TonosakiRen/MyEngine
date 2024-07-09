#pragma once
#include "GPUResource.h"
#include "DefaultBuffer.h"
#include "UAVBuffer.h"
#include "DescriptorHandle.h"

class CommandContext;

class RwStructuredBuffer : public UAVBuffer {
public:
    ~RwStructuredBuffer();

    void Create(const std::wstring& name, size_t bufferSize, UINT numElements);

    size_t GetBufferSize() const { return bufferSize_; }

    const DescriptorHandle& GetUAV() const {
        return uavHandle_;
    }

protected:
    void Destroy();

    DescriptorHandle uavHandle_;
};