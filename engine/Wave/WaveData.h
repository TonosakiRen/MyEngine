#pragma once
#include <Windows.h>

#include "GPUResource/StructuredBuffer.h"
#include "Graphics/CommandContext.h"
#include "Mymath.h"

class WaveData
{
public:
	const uint32_t kWaveNum;

	struct Data {
		Vector3 position;
		float t;
	};

	WaveData(uint32_t waveNum);
	void Initialize();

	const WaveData::Data* GetData() const{
		return data_;
	}

	const DescriptorHandle& GetGPUHandle() const { return structuredBuffer_.GetSRV(); }
	
	void Copy(const void* srcData, size_t copySize) { structuredBuffer_.Copy(srcData, copySize); }
	template<class T>
	void Copy(const T& srcData) { structuredBuffer_.Copy(&srcData, sizeof(srcData)); }

private:
	StructuredBuffer structuredBuffer_;
	Data* data_ = nullptr;
};