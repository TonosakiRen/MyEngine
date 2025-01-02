#pragma once
/**
 * @file WaveData.h
 * @brief 波を発生させるデータ
 */
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

	void Copy(const void* srcData, size_t copySize) { structuredBuffer_.Copy(srcData, copySize); }
	template<class T>
	void Copy(const T& srcData) { structuredBuffer_.Copy(&srcData, sizeof(srcData)); }

	//Getter
	const DescriptorHandle& GetGPUHandle() const { return structuredBuffer_.GetSRV(); }
	const WaveData::Data* GetData() const{
		return data_;
	}
private:
	StructuredBuffer structuredBuffer_;
	Data* data_ = nullptr;
};