#pragma once
/**
 * @file CopyBuffer.h
 * @brief コピー用buffer
 */
#include "GPUResource/GPUResource.h"

class CopyBuffer : public GPUResource {
public:
   
    void Create(const std::wstring& name, size_t bufferSize,void** cpuData);
    void Create(const std::wstring& name, size_t bufferSize);
    void Create(const std::wstring& name, size_t numElements, size_t elementSize) {
        Create(name, numElements * elementSize);
    }

    void Copy(const void* srcData, size_t copySize) const;
    template<class T>
    void Copy(const T& srcData) const { CopyResourceCopy(&srcData, sizeof(srcData)); }

    //データに0をセット
    void SetZero();
    //Getter
    size_t GetBufferSize() const { return bufferSize_; }
    void* GetCPUData() const { return cpuData_; }

protected:

    size_t bufferSize_ = 0;
    void* cpuData_ = nullptr;
};