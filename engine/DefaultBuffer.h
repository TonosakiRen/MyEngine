#pragma once
#include "GPUResource.h"


class DefaultBuffer : public GPUResource {
public:
    ~DefaultBuffer();

    void Create(size_t bufferSize);
    void Create(size_t numElements, size_t elementSize);

    size_t GetBufferSize() const { return bufferSize_; }

protected:
    void Destroy();

    size_t bufferSize_ = 0;
};