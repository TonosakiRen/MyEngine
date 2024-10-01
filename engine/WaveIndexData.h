#pragma once
#include <d3d12.h>
#include "Mymath.h"
#include <numbers>
#include "UploadBuffer.h"
#include "StructuredBuffer.h"

class WaveIndexData {
public:

	static const uint32_t kMaxInfluenceWaveNum = 255;

	void Initialize();
	void Update();

	const DescriptorHandle& GetGPUHandle() const {
		return waveIndexBuffer_.GetSRV();
	}
	std::vector<uint32_t> index_;
private:
	StructuredBuffer waveIndexBuffer_;
};