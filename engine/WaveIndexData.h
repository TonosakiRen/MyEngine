#pragma once
#include <d3d12.h>
#include "Mymath.h"
#include <numbers>
#include "UploadBuffer.h"

class WaveIndexData {
public:

	static const uint32_t kMaxInfluenceWaveNum = 5;

	struct ConstBufferData {
		uint32_t waveDataNum;
		uint32_t waveIndex[kMaxInfluenceWaveNum];
	};

	void Initialize();
	ConstBufferData* GetData() { return static_cast<ConstBufferData*>(constBuffer_.GetCPUData()); }

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return constBuffer_.GetGPUVirtualAddress();
	}
private:
	UploadBuffer constBuffer_;
};