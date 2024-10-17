#pragma once
#include "Particle/ParticleModelData.h"
#include "GameComponent/WorldTransform.h"
#include <vector>
#include <memory>
class PointLights;
class SphereLights
{
public:
	static const uint32_t kSphereNum = 64 * 2;
	void Initialize();
	void Update();
	void Draw(Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	struct Sphere {
		WorldTransform worldTransform_{};
		uint32_t pointLightIndex_;
		bool isActive_;
	};
public:
	std::vector<Sphere> spheres_;
	std::unique_ptr<ParticleModelData> particle_;
	PointLights* pointLights_;
	uint32_t activeNum_ = 0;
private:
};

