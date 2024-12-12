#pragma once
#include <memory>
#include "Particle/ParticleData.h"
#include "GameComponent/WorldTransform.h"
#include "GameComponent/Material.h"
class WhiteParticle
{
public:
	static const uint32_t kParticleNum = 300;
	WhiteParticle();
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
	struct DeadLineParticle {
		WorldTransform worldTransform;
		Vector3 direction;
		Vector3 velocity;
		bool isActive;
	};
public:
	WorldTransform emitterWorldTransform_;
	OBB emitBox_;
	DeadLineParticle particles[kParticleNum];
	int EmitNum_ = 1;
private:
	float speed_ = 0.05f;
	float scaleSpeed_ = 0.03f;
	bool isEmit_ = false;
	std::unique_ptr<ParticleData> particleData_;
	Vector3 minDirection_;
	Vector3 maxDirection_;
	Material material_;
};

