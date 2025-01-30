/**
 * @file MushroomParticle.cpp
 * @brief mushroomParticle
 */
#include "Particle/MushroomParticle.h"
#include "ImGuiManager.h"
#include "Draw/DrawManager.h"

void MushroomParticle::Initialize(WorldTransform& worldTransform)
{
	emitSphere_.center = MakeTranslation(worldTransform.matWorld_);
	emitSphere_.radius = 1.0f;
	material_.Initialize();
	material_.enableLighting_ = false;
	material_.Update();

	//洞窟の真ん中に向かうベクトルXY軸
	Vector3 centerXY = Normalize(Vector3{ -worldTransform.translation_.x ,-worldTransform.translation_.y,0.0f });
	const float randomOffset = 0.2f;
	Vector3 min = {centerXY.x + randomOffset ,centerXY.y + randomOffset,-randomOffset };
	Vector3 max = { centerXY.x + randomOffset ,centerXY.y + randomOffset, randomOffset };
	SetDirection(min,max);
}

void MushroomParticle::Update() {


	//出現
	if (isEmit_) {
		const float scale = 1.0f;
		for (size_t i = 0; i < EmitNum_; i++) {
			for (size_t j = 0; j < kParticleNum; j++) {
				if (particles[j].isActive == false) {
					particles[j].isActive = true;

					particles[j].direction = Normalize(Vector3{ Rand(minDirection_.x, maxDirection_.x) ,Rand(minDirection_.y,maxDirection_.y) ,Rand(minDirection_.z,maxDirection_.z) });

					particles[j].worldTransform.translation_ = MakeRandVector3(emitSphere_);
					particles[j].worldTransform.quaternion_ = IdentityQuaternion();
					particles[j].worldTransform.scale_ = Vector3{ scale ,scale ,scale };
					break;
				}
			}
		}
	}

	//更新
	for (size_t i = 0; i < kParticleNum; i++) {
		if (particles[i].isActive == true) {
			particles[i].worldTransform.translation_ += particles[i].direction * speed_;
			particles[i].worldTransform.scale_ = particles[i].worldTransform.scale_ - scaleSpeed_;
			particles[i].worldTransform.Update();
			if (particles[i].worldTransform.scale_.x <= 0.0f) {
				particles[i].isActive = false;
			}
		}
	}

}

void MushroomParticle::PushDataParticle(std::unique_ptr<ParticleData>& particleData)
{
	for (size_t i = 0; i < kParticleNum; i++)
	{
		if (particles[i].isActive) {
			ParticleData::Data data;
			data.matWorld = particles[i].worldTransform.matWorld_;
			data.worldInverseTranspose = Inverse(particles[i].worldTransform.matWorld_);
			particleData->PushBackData(data);
		}
	}
}
