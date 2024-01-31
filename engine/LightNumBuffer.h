#pragma once
#include "UploadBuffer.h"
#include <stdint.h>
class LightNumBuffer
{
public:
	void Create();

	struct LightNum {
		uint32_t directionalLightNum;
		uint32_t pointLightNum;
		uint32_t spotLightNum;
		uint32_t shadowSpotLightNum;
	};

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return lightNumBuffer_.GetGPUVirtualAddress();
	}

	UploadBuffer lightNumBuffer_;
};

