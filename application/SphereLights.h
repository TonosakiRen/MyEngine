#pragma once
#include "ParticleModel.h"
#include <vector>
class PointLights;
class SphereLights
{
public:
	static const uint32_t kSphereNum = 128;
	SphereLights();
	void Initialize(PointLights* pointLights);
	void Update();
	void Draw(Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	struct Sphere {
		WorldTransform worldTransform_{};
		uint32_t pointLightIndex_;
		bool isActive_;
	};
public:
	std::vector<Sphere> spheres_;
	std::unique_ptr<ParticleModel> particle_;
	PointLights* pointLights_;
	uint32_t activeNum_ = 0;
private:
};

