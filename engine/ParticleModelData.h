#pragma once
#include <Windows.h>

#include "StructuredBuffer.h"
#include "CommandContext.h"
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
	void PushBackData(const Data& data);
	void Reset();

	const DescriptorHandle& GetGPUHandle() const { return structuredBuffer_->GetSRV(); }
	const uint32_t GetDataNum() const { return dataNum_; }
private:
	std::unique_ptr<StructuredBuffer> structuredBuffer_;
	Data* data_ = nullptr;
	uint32_t dataNum_ = 0;
};