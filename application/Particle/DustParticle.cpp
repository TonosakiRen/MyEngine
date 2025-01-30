/**
 * @file DustParticle.h
 * @brief 足元のパーティクル
 */
#include "Particle/DustParticle.h"
#include "Draw/DrawManager.h"
#include "ImGuiManager.h"

DustParticle::DustParticle()
{
	particleData_ = std::make_unique<ParticleModelData>(kParticleNum);
}

void DustParticle::Initialize(Vector3 minDirection, Vector3 maxDirection)
{

	particleData_->Initialize();
	material_.Initialize();
	emitterWorldTransform_.SetIsScaleParent(false);
	emitterWorldTransform_.Update();
	SetDirection(minDirection, maxDirection);
}

void DustParticle::Update() {

	emitterWorldTransform_.Update();

	//出現
	if (isEmit_) {
		for (size_t i = 0; i < EmitNum_; i++) {
			for (size_t j = 0; j < kParticleNum; j++) {
				if (particles[j].isActive_ == false) {
					particles[j].isActive_ = true;
					if (emitterWorldTransform_.GetParent()) {
						particles[j].direction_ = Normalize(Vector3{ Rand(minDirection_.x, maxDirection_.x) ,Rand(minDirection_.y,maxDirection_.y) ,Rand(minDirection_.z,maxDirection_.z) } *NormalizeMakeRotateMatrix(emitterWorldTransform_.GetParent()->matWorld_));
					}
					else {
						particles[j].direction_ = Normalize(Vector3{ Rand(minDirection_.x, maxDirection_.x) ,Rand(minDirection_.y,maxDirection_.y) ,Rand(minDirection_.z,maxDirection_.z) });
					}
					particles[j].worldTransform_.translation_ = MakeTranslation(emitterWorldTransform_.matWorld_);
					particles[j].worldTransform_.quaternion_ = IdentityQuaternion();
					particles[j].worldTransform_.scale_ = emitterWorldTransform_.scale_;
					break;
				}
			}
		}
	}

	//更新
	for (size_t i = 0; i < kParticleNum; i++) {
		float rotationSpeed = Radian(1.0f) * (float(i % 2) * 2.0f - 1.0f);
		if (particles[i].isActive_ == true) {
			particles[i].worldTransform_.quaternion_ *= MakeFromAngleAxis({ 1.0f,1.0f,1.0f }, rotationSpeed);
			particles[i].worldTransform_.translation_ += particles[i].direction_ * speed_;
			particles[i].worldTransform_.scale_ = particles[i].worldTransform_.scale_ - scaleSpeed_;
			particles[i].worldTransform_.Update();
			if (particles[i].worldTransform_.scale_.x <= 0.0f) {
				particles[i].isActive_ = false;
			}
		}

	}

}

void DustParticle::Draw()
{
	material_.Update();

	for (size_t i = 0; i < kParticleNum; i++)
	{
		if (particles[i].isActive_) {
			ParticleModelData::Data data;
			data.matWorld = particles[i].worldTransform_.matWorld_;
			data.worldInverseTranspose = Inverse(particles[i].worldTransform_.matWorld_);
			particleData_->PushBackData(data);
		}
	}

	Engine::DrawManager::GetInstance()->DrawParticleModel(*particleData_,0, material_);
}