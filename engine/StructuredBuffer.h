#pragma once
#include "UploadBuffer.h"
#include "DescriptorHandle.h"
class StructuredBuffer
	:public UploadBuffer
{
public:
	void Create(size_t bufferSize, UINT numElements);
private:
	DescriptorHandle srvHandle_;
};

