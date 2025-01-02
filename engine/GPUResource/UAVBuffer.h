#pragma once
/**
 * @file UAVBuffer.h
 * @brief UAVBuffer
 */
#include "GPUResource/GPUResource.h"


class UAVBuffer : public GPUResource {
public:

    void Create(const std::wstring& name, size_t bufferSize);
    void Create(const std::wstring& name, size_t numElements, size_t elementSize);

    //Getter
    size_t GetBufferSize() const { return bufferSize_; }

protected:

    size_t bufferSize_ = 0;
};