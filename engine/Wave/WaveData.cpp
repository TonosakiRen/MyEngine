/**
 * @file WaveData.h
 * @brief 波を発生させるデータ
 */
#include "Wave/WaveData.h"
#include "Particle/Particle.h"
#include "Light/ShaderManager.h"
#include "Render/Renderer.h"
#include "Texture/TextureManager.h"

using namespace Microsoft::WRL;

WaveData::WaveData(uint32_t waveNum) : kWaveNum(waveNum) {
}

void WaveData::Initialize()
{
	structuredBuffer_.Create(L"waveDataBuffer", sizeof(Data), kWaveNum);
	data_ = static_cast<Data*>(structuredBuffer_.GetCPUData());
}
