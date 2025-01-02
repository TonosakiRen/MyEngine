/**
 * @file RwStructuredBuffer.h
 * @brief read,write用StructuredBuffer
 */
#pragma once
#include "GPUResource/GPUResource.h"
#include "GPUResource/DefaultBuffer.h"
#include "GPUResource/UAVBuffer.h"
#include "Graphics/DescriptorHandle.h"

class CommandContext;

class RwStructuredBuffer : public UAVBuffer {
public:

    void Create(const std::wstring& name, size_t bufferSize, UINT numElements);

    //Getter
    size_t GetBufferSize() const { return bufferSize_; }
    const DescriptorHandle& GetUAV() const {
        return uavHandle_;
    }

protected:

    DescriptorHandle uavHandle_;
};