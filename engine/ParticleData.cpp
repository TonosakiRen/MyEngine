#include "ParticleData.h"
#include "Particle.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "TextureManager.h"

using namespace Microsoft::WRL;

ParticleData::ParticleData(uint32_t particleNum) : kParticleNum(particleNum) {
}

void ParticleData::Initialize()
{
	structuredBuffer_.Create(sizeof(Data), kParticleNum);
	data_ = static_cast<Data*>(structuredBuffer_.GetCPUData());
}

void ParticleData::PushBackData(const Data& data)
{

	data_[dataNum_] = data;
	dataNum_++;
}

void ParticleData::Reset()
{
	dataNum_ = 0;
}
