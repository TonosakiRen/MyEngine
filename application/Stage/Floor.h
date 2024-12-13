#pragma once
/**
 * @file Floor.h
 * @brief 床
 */
#include "GameComponent/GameObject.h"
#include "Wave/WaveIndexData.h"
#include "WavePoints/WavePoints.h"
class Floor :
    public GameObject
{
public:

    static const float kFloorSize;
    static const float kFloorHalfSize;

    void Initialize(const std::string name, WavePoints* wavePoints);
    void Update();
    void Draw();
    WaveIndexData waveIndexData_;
private:
    uint32_t textureHandle_ = 0;
    WavePoints* wavePoints_ = nullptr;
};