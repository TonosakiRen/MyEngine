#pragma once
/**
 * @file ParticleData.h
 * @brief ParticleのためのBuffer
 */
#include <Windows.h>

#include "GPUResource/StructuredBuffer.h"
#include "Graphics/CommandContext.h"
#include "Mymath.h"

class ParticleData
{
public:

	const uint32_t kParticleNum;

	struct Data {
		Matrix4x4 matWorld;
		Matrix4x4 worldInverseTranspose;
	};

	ParticleData();
	ParticleData(uint32_t particleNum);
	void Initialize();
	//Dataを追加
	void PushBackData(const Data& data);
	//Dataをリセット
	void Reset();
	//Getter
	const DescriptorHandle& GetGPUHandle() const { return structuredBuffer_->GetSRV(); }
	const uint32_t GetDataNum() const { return dataNum_; }

private:
	std::unique_ptr<StructuredBuffer> structuredBuffer_;
	Data* data_ = nullptr;
	uint32_t dataNum_ = 0;
};