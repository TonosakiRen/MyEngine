/**
 * @file WaveIndexData.cpp
 * @brief 波を発生させるデータのIndexのデータ
 */
#include "Wave/WaveIndexData.h"
#include "ImGuiManager.h"

void WaveIndexData::Initialize() {
    waveIndexBuffer_.Create(L"waveIndexDataBuffer",sizeof(uint32_t), kMaxInfluenceWaveNum + 1);
}

void WaveIndexData::Update()
{
    
    uint32_t* data = static_cast<uint32_t*>(waveIndexBuffer_.GetCPUData());
    *data = static_cast<uint32_t>(index_.size());

    for (uint32_t i = 1; uint32_t index : index_) {
        data[i] = index;
        i++;
    }
}
