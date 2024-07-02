#pragma once
#include "UploadBuffer.h"
#include "DescriptorHandle.h"
#include "CommandContext.h"
class ByteAddressBuffer
	: public GPUResource
{
public:
    ~ByteAddressBuffer();

	void Create(size_t bufferSize, UINT numElements);
	const DescriptorHandle& GetSRV() const { return srvHandle_; }


    void Copy(const void* srcData, size_t copySize, CommandContext& commandContext);
    template<class T>
    void Copy(const T& srcData, CommandContext& commandContext) { Copy(&srcData, sizeof(srcData), commandContext); }

    size_t GetBufferSize() const { return bufferSize_; }
    void* GetCPUData() const { return cpuData_; }

    void SetZero();

protected:
    void Destroy();

    size_t bufferSize_ = 0;
    void* cpuData_ = nullptr;
	DescriptorHandle srvHandle_;
    UploadBuffer copyBuffer_;
};
