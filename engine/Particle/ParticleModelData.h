#pragma once
/**
 * @file ParticleModelData.h
 * @brief ParticleModelのためのBuffer
 */
#include <Windows.h>

#include "GPUResource/StructuredBuffer.h"
#include "Graphics/CommandContext.h"
#include "Mymath.h"

class ParticleModelData
{
public:

	const uint32_t kParticleNum;

	struct Data {
		Matrix4x4 matWorld{};
		Matrix4x4 worldInverseTranspose{};
		Vector4 color = {1.0f,1.0,1.0f,1.0f};
	};

	ParticleModelData(uint32_t particleNum);
	void Initialize();
	//Dataを追加
	void PushBackData(const Data& data);
	//Dataをリセット
	void Reset();

	const DescriptorHandle& GetGPUHandle() const { return structuredBuffer_->GetSRV(); }
	const uint32_t GetDataNum() const { return dataNum_; }
	const Data* GetData() const { return data_; }
private:
	std::unique_ptr<StructuredBuffer> structuredBuffer_;
	Data* data_ = nullptr;
	uint32_t dataNum_ = 0;
};