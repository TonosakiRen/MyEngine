#pragma once
#include <memory>
#include "Particle/ParticleModelData.h"
#include "GameComponent/WorldTransform.h"
#include "Stage/Floor.h"
#include "WavePoints/WavePoints.h"
#include "Light/PointLights.h"
class ExplodeParticle
{
public:
	static const uint32_t kParticleNum = 256;
	ExplodeParticle();
	void Initialize(Floor* floor, WavePoints* wavePoints);
	void Update();
	void Draw();
	void SetIsEmit(bool isEmit) {
		isEmit_ = isEmit;
	}
	void SetIsEmit(bool isEmit,const Vector3& emitPos,const Vector4& color) {
		emitterWorldTransform_.translation_ = emitPos;
		emitColor_ = color;
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
		PointLight* pointLight_ = nullptr;
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
	float scaleSpeed_ = 0.005f;
	bool isEmit_ = false;
	std::unique_ptr<ParticleModelData> particle_;
	Vector3 acceleration_;
	float directionScalar_ = 0.06f;
	Vector2 initVelocity_ = { 0.2f,0.3f };
	Vector4 emitColor_ = {1.0f,1.0f,1.0f,1.0f};
	uint32_t modelHandle_ = 0;
	float boundNum_ = 0.63f;
};

