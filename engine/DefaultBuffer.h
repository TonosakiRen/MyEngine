#pragma once
#include "GPUResource.h"
#include "CopyBuffer.h"
#include "CommandContext.h"

class DefaultBuffer : public GPUResource {
public:

    void Create(const std::wstring& name, size_t bufferSize);
    void Create(const std::wstring& name, size_t numElements, size_t elementSize);


    void Copy(const void* srcData, size_t copySize, CommandContext& commandContext);
    template<class T>
    void Copy(const T& srcData, CommandContext& commandContext) { Copy(&srcData, sizeof(srcData), commandContext); }

    void DestroyCopyBuffer();

    size_t GetBufferSize() const { return bufferSize_; }

protected:
    CopyBuffer copyBuffer_;
    size_t bufferSize_ = 0;
};