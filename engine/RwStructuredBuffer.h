#pragma once
#include "GPUResource.h"
#include "DefaultBuffer.h"
#include "UAVBuffer.h"
#include "DescriptorHandle.h"

class CommandContext;

class RwStructuredBuffer : public UAVBuffer {
public:
    ~RwStructuredBuffer();

    void Create(size_t bufferSize, UINT numElements);

    size_t GetBufferSize() const { return bufferSize_; }

    DescriptorHandle GetUAV() {
        return uavHandle_;
    }

protected:
    void Destroy();

    DescriptorHandle uavHandle_;
    bool isCopy_ = false;
};