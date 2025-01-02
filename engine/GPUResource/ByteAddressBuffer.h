/**
 * @file ByteAddressBuffer.cpp
 * @brief Byte単位でアクセスるバッファー
 */
#pragma once
#include "GPUResource/CopyBuffer.h"
#include "Graphics/DescriptorHandle.h"
#include "Graphics/CommandContext.h"
class ByteAddressBuffer
	: public GPUResource
{
public:

	void Create(const std::wstring& name, size_t bufferSize, UINT numElements);
	const DescriptorHandle& GetSRV() const { return srvHandle_; }


    void Copy(const void* srcData, size_t copySize, CommandContext& commandContext);
    template<class T>
    void Copy(const T& srcData, CommandContext& commandContext) { Copy(&srcData, sizeof(srcData), commandContext); }

    //Setter
    void SetZero();
    //Getter
    size_t GetBufferSize() const { return bufferSize_; }
    void* GetCPUData() const { return cpuData_; }

    //Copy用Bufferを解放
    void DestroyCopyBuffer();

protected:

    size_t bufferSize_ = 0;
    void* cpuData_ = nullptr;
    bool isCopy_ = false;
	DescriptorHandle srvHandle_;
    CopyBuffer copyBuffer_;
};
