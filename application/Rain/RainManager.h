#pragma once
/**
 * @file RainManager.h
 * @brief 雨の管理
 */
#include "MyMath.h"
#include "Rain/RainDrop.h"
#include "Light/LightManager.h"
#include <array>

class RainManager
{
public:
	static Vector3 modelSize;
	static const uint32_t rainDropNum_ = 512;

	void Initialize(ExplodeParticle* explodeParticle);
	void Update();
	void Draw();
	//Getter
	std::array<RainDrop, rainDropNum_>& GetRainDrop() {
		return rainDrops_;
	}

private:
	void PopRainDrop();
private:
	ExplodeParticle* explodeParticle_ = nullptr;
	PointLights* pointLights_ = nullptr;
	std::unique_ptr<ParticleModelData> particle_;
	std::array<RainDrop, rainDropNum_> rainDrops_;
	std::unique_ptr < Material >material_;
	uint32_t spawnFrame_ = 0;
};

