#pragma once
#include "UploadBuffer.h"
#include "DescriptorHandle.h"
class StructuredBuffer
	:public UploadBuffer
{
public:
	void Create(size_t bufferSize, UINT numElements);
	const DescriptorHandle& GetSRV() const { return srvHandle_; }
private:
	DescriptorHandle srvHandle_;
};
