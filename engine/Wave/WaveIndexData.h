#pragma once
/**
 * @file WaveIndexData.h
 * @brief 波を発生させるデータのIndexのデータ
 */
#include <d3d12.h>
#include "Mymath.h"
#include <numbers>
#include "GPUResource/UploadBuffer.h"
#include "GPUResource/StructuredBuffer.h"

class WaveIndexData {
public:

	static const uint32_t kMaxInfluenceWaveNum = 255;

	void Initialize();
	void Update();

	const DescriptorHandle& GetGPUHandle() const {
		return waveIndexBuffer_.GetSRV();
	}

	std::vector<uint32_t>& GetIndex() {
		return index_;
	}

private:
	std::vector<uint32_t> index_;
	StructuredBuffer waveIndexBuffer_;
};