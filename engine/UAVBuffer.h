#pragma once
#include "GPUResource.h"


class UAVBuffer : public GPUResource {
public:

    void Create(const std::wstring& name, size_t bufferSize);
    void Create(const std::wstring& name, size_t numElements, size_t elementSize);

    size_t GetBufferSize() const { return bufferSize_; }

protected:

    size_t bufferSize_ = 0;
};