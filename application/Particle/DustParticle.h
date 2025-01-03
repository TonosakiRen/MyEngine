#pragma once
/**
 * @file DustParticle.h
 * @brief 足元のパーティクル
 */
#include <memory>
#include "Particle/ParticleModelData.h"
#include "GameComponent/WorldTransform.h"
#include "GameComponent/Material.h"
class DustParticle
{
public:
	static const uint32_t kParticleNum = 100;
	DustParticle();
	void Initialize(Vector3 minDirection, Vector3 maxDirection);
	void Update();
	void Draw();
	//Setter
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
	Material material_;
	int EmitNum_ = 1;
private:
	float speed_ = 0.1f;
	float scaleSpeed_ = 0.02f;
	bool isEmit_ = true;
	std::unique_ptr<ParticleModelData> particleData_;
	Vector3 minDirection_;
	Vector3 maxDirection_;

};

