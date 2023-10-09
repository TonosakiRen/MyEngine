#pragma once
#include "PixelBuffer.h"
#include "DescriptorHandle.h"
class DepthBuffer : public PixelBuffer
{
public:
	void Create(const std::wstring& name, uint32_t width, uint32_t height, DXGI_FORMAT format);
	float GetClearValue() const { return clearValue_; }
	const DescriptorHandle& GetDSV() const { return dsvHandle_; }
	const DescriptorHandle& GetSRV() const { return srvHandle_; }
private:
	void CreateViews();

	float clearValue_ = 1.0f;
	DescriptorHandle dsvHandle_;
	DescriptorHandle srvHandle_;
};

