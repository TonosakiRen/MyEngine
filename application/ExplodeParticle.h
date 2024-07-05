#pragma once
#include <memory>
#include "ParticleModelData.h"
#include "WorldTransform.h"
class PointLights;
class ExplodeParticle
{
public:
	static const uint32_t kParticleNum = 60;
	ExplodeParticle();
	void Initialize(Vector3 minDirection, Vector3 maxDirection);
	void Update();
	void Draw(Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	void SetDirection(Vector3 minDirection, Vector3 maxDirection) {
		minDirection_ = minDirection;
		maxDirection_ = maxDirection;
	}
	void SetIsEmit(bool isEmit) {
		isEmit_ = isEmit;
	}
	void SetIsEmit(bool isEmit,Vector3 emitPos) {
		emitterWorldTransform_.translation_ = emitPos;
		isEmit_ = isEmit;
	}
	void SetSpeed(float speed) {
		speed_ = speed;
	}
	void SetScaleSpeed(float scaleSpeed) {
		scaleSpeed_ = scaleSpeed;
	}
	struct Particle {
		WorldTransform worldTransform_;
		Vector3 direction_;
		Vector3 velocity_;
		uint32_t pointLightIndex_;
		bool isActive_;
	};
public:
	WorldTransform emitterWorldTransform_;
	Particle particles[kParticleNum];
	PointLights* pointLights_;
	int EmitNum_ = 20;
private:
	float speed_ = 0.1f;
	float scaleSpeed_ = 0.01f;
	bool isEmit_ = false;
	std::unique_ptr<ParticleModelData> particle_;
	Vector3 minDirection_;
	Vector3 maxDirection_;
	Vector3 acceleration_;
};

