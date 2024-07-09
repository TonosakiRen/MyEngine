#include "ParticleModelData.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "TextureManager.h"

using namespace Microsoft::WRL;

ParticleModelData::ParticleModelData(uint32_t particleNum) : kParticleNum(particleNum) {
}

void ParticleModelData::Initialize()
{
	structuredBuffer_.Create(L"particleModelDataBuffer", sizeof(Data), kParticleNum);
	data_ = static_cast<Data*>(structuredBuffer_.GetCPUData());
}

void ParticleModelData::PushBackData(const Data& data)
{
	data_[dataNum_] = data;
	dataNum_++;
}

void ParticleModelData::Reset()
{
	dataNum_ = 0;
}
