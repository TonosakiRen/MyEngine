#pragma once
#include "GPUResource/UploadBuffer.h"
#include "Graphics/DescriptorHandle.h"
#include "Framework/Framework.h"
class StructuredBuffer
	:public UploadBuffer
{
public:
	void Create(const std::wstring& name, size_t bufferSize, UINT numElements);

	const DescriptorHandle& GetSRV() const { return srvHandle_[Framework::kFrameRemainder]; }
private:
	DescriptorHandle srvHandle_[2];
};
