/**
 * @file ParticleModelData.cpp
 * @brief ParticleModelのためのBuffer
 */
#include "Particle/ParticleModelData.h"
#include "Light/ShaderManager.h"
#include "Render/Renderer.h"
#include "Texture/TextureManager.h"

using namespace Microsoft::WRL;

ParticleModelData::ParticleModelData(uint32_t particleNum) : kParticleNum(particleNum) {
}

void ParticleModelData::Initialize()
{
	structuredBuffer_ = std::make_unique<StructuredBuffer>();
	structuredBuffer_->Create(L"particleModelDataBuffer", sizeof(Data), kParticleNum);
	data_ = static_cast<Data*>(structuredBuffer_->GetCPUData());
	
}

void ParticleModelData::PushBackData(const Data& data)
{
	data_ = static_cast<Data*>(structuredBuffer_->GetCPUData());
	data_[dataNum_] = data;
	dataNum_++;
}

void ParticleModelData::Reset()
{
	dataNum_ = 0;
}
