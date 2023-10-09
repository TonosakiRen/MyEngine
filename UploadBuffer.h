#pragma once
#include "GPUResource.h"

#include <string>
class UploadBuffer
	: public GPUResource
{
public:
	~UploadBuffer();
	void Create(const std::wstring& name, size_t bufferSize);
	void Copy(const void* srcData, size_t copySize);

	size_t GetBufferSize() const { return bufferSize_; }
	void* GetCPUData() const { return cpuData_; }
protected:
	void Destroy();
	size_t bufferSize_ = 0;
	void* cpuData_;
};

