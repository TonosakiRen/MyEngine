#pragma once
/**
 * @file ExplodeParticle.h
 * @brief 爆発パーティクル
 */
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
	
	struct Particle {
		WorldTransform worldTransform_;
		Vector3 velocity_;
		PointLight* pointLight_ = nullptr;
		Vector4 color_;
		bool firstDrop_ = false;
		bool isActive_ = false;
	};
private:
	Floor* floor_ = nullptr;
	WavePoints* wavePoints_ = nullptr;
	PointLights* pointLights_ = nullptr;

	WorldTransform emitterWorldTransform_;
	Particle particles[kParticleNum];

	const int EmitNum_ = 20;
	const float speed_ = 0.1f;
	const float scaleSpeed_ = 0.005f;
	const float directionScalar_ = 0.06f;
	const Vector2 initVelocity_ = { 0.2f,0.3f };
	const float boundNum_ = 0.63f;

	bool isEmit_ = false;
	std::unique_ptr<ParticleModelData> particle_;
	Vector3 acceleration_;
	Vector4 emitColor_ = { 1.0f,1.0f,1.0f,1.0f };
	uint32_t modelHandle_ = 0;
};

