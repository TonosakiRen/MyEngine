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
		Matrix4x4 matWorld;
		Matrix4x4 worldInverseTranspose;
	};

	ParticleModelData(uint32_t particleNum);
	void Initialize();
	void PushBackData(const Data& data);
	void Reset();

public:
	StructuredBuffer structuredBuffer_;
	Data* data_ = nullptr;
	uint32_t dataNum_ = 0;
};