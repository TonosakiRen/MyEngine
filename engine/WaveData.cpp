#include "WaveData.h"
#include "Particle.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "TextureManager.h"

using namespace Microsoft::WRL;

WaveData::WaveData(uint32_t waveNum) : kWaveNum(waveNum) {
}

void WaveData::Initialize()
{
	structuredBuffer_.Create(L"waveDataBuffer", sizeof(Data), kWaveNum);
	data_ = static_cast<Data*>(structuredBuffer_.GetCPUData());
}
