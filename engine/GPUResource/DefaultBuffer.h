#pragma once
/**
 * @file DefaultBuffer.h
 * @brief DefaultHeap用Buffer
 */
#include "GPUResource/GPUResource.h"
#include "GPUResource/CopyBuffer.h"
#include "Graphics/CommandContext.h"

class DefaultBuffer : public GPUResource {
public:

    void Create(const std::wstring& name, size_t bufferSize);
    void Create(const std::wstring& name, size_t numElements, size_t elementSize);


    void Copy(const void* srcData, size_t copySize, CommandContext& commandContext);
    template<class T>
    void Copy(const T& srcData, CommandContext& commandContext) { Copy(&srcData, sizeof(srcData), commandContext); }

    //Copy専用Bufferを解放
    void DestroyCopyBuffer();
    //Getter
    size_t GetBufferSize() const { return bufferSize_; }

protected:
    CopyBuffer copyBuffer_;
    size_t bufferSize_ = 0;
};