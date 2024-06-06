#pragma once
#include <memory>
#include "ParticleModelData.h"
#include "WorldTransform.h"
class DustParticle
{
public:
	static const uint32_t kParticleNum = 100;
	DustParticle();
	void Initialize(Vector3 minDirection, Vector3 maxDirection);
	void Update();
	void Draw();
	void SetDirection(Vector3 minDirection, Vector3 maxDirection) {
		minDirection_ = minDirection;
		maxDirection_ = maxDirection;
	}
	void SetIsEmit(bool isEmit) {
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
		bool isActive_;
	};
public:
	WorldTransform emitterWorldTransform_;
	Particle particles[kParticleNum];
	int EmitNum_ = 1;
private:
	float speed_ = 0.1f;
	float scaleSpeed_ = 0.01f;
	bool isEmit_ = false;
	std::unique_ptr<ParticleModelData> particleData_;
	Vector3 minDirection_;
	Vector3 maxDirection_;
};

