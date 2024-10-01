#pragma once
#include <memory>
#include "ParticleModelData.h"
#include "WorldTransform.h"
#include "Floor.h"
#include "WavePoints.h"
class PointLights;
class ExplodeParticle
{
public:
	static const uint32_t kParticleNum = 128;
	ExplodeParticle();
	void Initialize(Floor* floor, WavePoints* wavePoints);
	void Update();
	void Draw(Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
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
		Vector3 velocity_;
		uint32_t pointLightIndex_ = 0;
		Vector4 color_;
		bool firstDrop_ = false;
		bool isActive_ = false;
	};
public:
	WorldTransform emitterWorldTransform_;
	Particle particles[kParticleNum];
	PointLights* pointLights_ = nullptr;
	int EmitNum_ = 20;
private:
	Floor* floor_;
	WavePoints* wavePoints_;
	float speed_ = 0.1f;
	float scaleSpeed_ = 0.01f;
	bool isEmit_ = false;
	std::unique_ptr<ParticleModelData> particle_;
	Vector3 acceleration_;
	float directionScalar_ = 0.06f;
	Vector2 initVelocity_ = { 0.2f,0.3f };
	float boundNum_ = 0.63f;
};

