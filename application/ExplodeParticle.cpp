#include "ExplodeParticle.h"
#include "ImGuiManager.h"
#include "DrawManager.h"
#include "LightManager.h"

ExplodeParticle::ExplodeParticle()
{
	particle_ = std::make_unique<ParticleModelData>(kParticleNum);
}

void ExplodeParticle::Initialize(Vector3 minDirection, Vector3 maxDirection)
{
	pointLights_ = LightManager::GetInstance()->pointLights_.get();
	acceleration_ = { 0.0f,0.0f,-0.1f };
	particle_->Initialize();
	emitterWorldTransform_.SetIsScaleParent(false);
	emitterWorldTransform_.Update();
	SetDirection(minDirection, maxDirection);
}

void ExplodeParticle::Update() {

	emitterWorldTransform_.Update();
	const float pointLightRadius_ = 1.0f;

	/*if (isEmit_) {
		for (size_t i = 0; i < EmitNum_; i++) {
			for (size_t i = 0; i < kParticleNum; i++) {
				if (particles[i].isActive_ == false) {
					particles[i].isActive_ = true;
					particles[i].direction_ = Normalize(Vector3{ Rand(minDirection_.x, maxDirection_.x) ,Rand(minDirection_.y,maxDirection_.y) ,Rand(minDirection_.z,maxDirection_.z) });
					particles[i].worldTransform_.translation_ = MakeTranslation(emitterWorldTransform_.matWorld_);
					particles[i].worldTransform_.quaternion_ = IdentityQuaternion();
					particles[i].worldTransform_.scale_ = emitterWorldTransform_.scale_;
					particles[i].worldTransform_.Update();
					for (size_t j = 0; j < PointLights::lightNum; j++) {
						if (pointLights_->lights_[j].isActive == false) {
							pointLights_->lights_[j].worldTransform.translation_ = {0.0f,0.0f,0.0f};
							pointLights_->lights_[j].worldTransform.parent_ = &particles[i].worldTransform_;
							pointLights_->lights_[j].isActive = true;
							pointLights_->lights_[j].color = HSVA(Rand(0.0f, 1.0f),1.0f,1.0f,1.0f);
							pointLights_->lights_[j].radius = pointLightRadius_;
							pointLights_->lights_[j].intensity = 3.0f;
							pointLights_->lights_[j].decay = 1.0f;
							particles[i].pointLightIndex_ = static_cast<uint32_t>(j);
							break;
						}
					}
					break;
				}
			}
		}
		isEmit_ = false;
	}*/

	for (size_t i = 0; i < kParticleNum; i++) {
		float rotationSpeed = Radian(1.0f) * (float(i % 2) * 2.0f - 1.0f);
		if (particles[i].isActive_ == true) {
			particles[i].worldTransform_.quaternion_ *= MakeFromAngleAxis({ 1.0f,1.0f,1.0f }, rotationSpeed);
			particles[i].worldTransform_.translation_ += particles[i].direction_ * speed_;
			particles[i].worldTransform_.scale_ = particles[i].worldTransform_.scale_ - scaleSpeed_;
			pointLights_->lights_[particles[i].pointLightIndex_].intensity = 3.0f * (particles[i].worldTransform_.scale_.x / emitterWorldTransform_.scale_.x);
			pointLights_->lights_[particles[i].pointLightIndex_].worldTransform.Update();
			if (particles[i].worldTransform_.scale_.x <= 0.0f) {
				particles[i].isActive_ = false;
				pointLights_->lights_[particles[i].pointLightIndex_].isActive = false;
			}
		}

	}

}

void ExplodeParticle::Draw(Vector4 color)
{

	emitterWorldTransform_.Update();

	for (size_t i = 0; i < kParticleNum; i++)
	{
		if (particles[i].isActive_) {
			ParticleModelData::Data data;
			data.matWorld = particles[i].worldTransform_.matWorld_;
			particle_->PushBackData(data);
		}
	}

	DrawManager::GetInstance()->DrawParticleModel(*particle_);
}