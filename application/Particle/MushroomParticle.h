#pragma once
/**
 * @file MushroomParticle.h
 * @brief mushroomParticle
 */
#include <memory>
#include "Particle/ParticleData.h"
#include "GameComponent/WorldTransform.h"
#include "GameComponent/Material.h"
class MushroomParticle
{
public:
	static const uint32_t kParticleNum = 1;
	void Initialize(WorldTransform& worldTransform);
	void Update();
	//particleの情報を送る
	void PushDataParticle(std::unique_ptr<ParticleData>& particleData);
	//Setter
	void SetDirection(Vector3 minDirection, Vector3 maxDirection) {
		minDirection_ = minDirection;
		maxDirection_ = maxDirection;
	}
	void SetIsEmit(bool isEmit) {
		isEmit_ = isEmit;
	}
	void SetColor(const Vector4& color) {
		color_ = color;
	}
	struct Particle {
		WorldTransform worldTransform;
		Vector3 direction;
		Vector3 velocity;
		bool isActive;
	};
private:
	const int EmitNum_ = 1;
	const float speed_ = 0.05f;
	const float scaleSpeed_ = 0.03f;

	Sphere emitSphere_;
	Particle particles[kParticleNum];
	bool isEmit_ = true;
	Vector3 minDirection_;
	Vector3 maxDirection_;
	Material material_;
	Vector4 color_ = {1.0f,1.0f,1.0f,1.0f};
};

