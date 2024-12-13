#pragma once
/**
 * @file SphereLights.h
 * @brief sphereLight
 */
#include "Particle/ParticleModelData.h"
#include "GameComponent/WorldTransform.h"
#include "GameComponent/Material.h"
#include <array>
#include <memory>
class PointLights;
class SphereLights
{
public:
	static const uint32_t kSphereNum = 64;
	void Initialize();
	void Emit(const Vector3& position, const Vector4& color);
	void Update();
	void Draw();
	struct Sphere {
		WorldTransform worldTransform_{};
		Vector4 color_;
		uint32_t pointLightIndex_;
		float intensity;
		bool isActive_;
	};
private:
	std::array<Sphere, kSphereNum> spheres_;
	std::unique_ptr<ParticleModelData> particle_;
	std::unique_ptr<Material> material_;
	const float intensity_ = 3.0f;
	const float radius_ = 6.0f;
	PointLights* pointLights_ = nullptr;
	uint32_t activeNum_ = 0;

};

