#pragma once
#include "CopyBuffer.h"
#include "DescriptorHandle.h"
#include "CommandContext.h"
class ByteAddressBuffer
	: public GPUResource
{
public:

	void Create(const std::wstring& name, size_t bufferSize, UINT numElements);
	const DescriptorHandle& GetSRV() const { return srvHandle_; }


    void Copy(const void* srcData, size_t copySize, CommandContext& commandContext);
    template<class T>
    void Copy(const T& srcData, CommandContext& commandContext) { Copy(&srcData, sizeof(srcData), commandContext); }

    size_t GetBufferSize() const { return bufferSize_; }
    void* GetCPUData() const { return cpuData_; }

    void SetZero();

    void DestroyCopyBuffer();

protected:

    size_t bufferSize_ = 0;
    void* cpuData_ = nullptr;
    bool isCopy_ = false;
	DescriptorHandle srvHandle_;
    CopyBuffer copyBuffer_;
};
