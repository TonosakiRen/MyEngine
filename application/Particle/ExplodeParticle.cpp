#include "Particle/ExplodeParticle.h"
#include "ImGuiManager.h"
#include "Draw/DrawManager.h"
#include "Light/LightManager.h"

ExplodeParticle::ExplodeParticle()
{
	particle_ = std::make_unique<ParticleModelData>(kParticleNum);
	pointLights_ = LightManager::GetInstance()->pointLights_.get();
}

void ExplodeParticle::Initialize(Floor* floor, WavePoints* wavePoints)
{
	floor_ = floor;
	wavePoints_ = wavePoints;
	acceleration_ = { 0.0f,-0.01f,0.0f };
	particle_->Initialize();
	emitterWorldTransform_.SetIsScaleParent(false);
	emitterWorldTransform_.scale_ = { 0.8f,0.8f,0.8f };
	emitterWorldTransform_.Update();
}

void ExplodeParticle::Update() {

	emitterWorldTransform_.Update();
	const float pointLightRadius_ = 3.0f;

	if (isEmit_) {
		for (size_t i = 0; i < EmitNum_; i++) {
			for (size_t i = 0; i < kParticleNum; i++) {
				if (particles[i].isActive_ == false) {
					particles[i].isActive_ = true;
					particles[i].velocity_ = { Rand(-directionScalar_,directionScalar_),Rand(initVelocity_.x,initVelocity_.y), Rand(-directionScalar_,directionScalar_) };
					particles[i].worldTransform_.translation_ = MakeTranslation(emitterWorldTransform_.matWorld_);
					particles[i].worldTransform_.quaternion_ = IdentityQuaternion();
					particles[i].worldTransform_.scale_ = emitterWorldTransform_.scale_;
					particles[i].worldTransform_.Update();
					particles[i].firstDrop_ = false;
					for (size_t j = 0; j < PointLights::lightNum; j++) {
						if (pointLights_->lights_[j].isActive == false) {
							pointLights_->lights_[j].worldTransform.translation_ = {0.0f,0.0f,0.0f};
							pointLights_->lights_[j].worldTransform.SetParent(&particles[i].worldTransform_);
							pointLights_->lights_[j].isActive = true;
							pointLights_->lights_[j].color = HSVA(Rand(0.0f, 1.0f),1.0f,1.0f,1.0f);
							pointLights_->lights_[j].radius = pointLightRadius_;
							pointLights_->lights_[j].intensity = 3.0f;
							pointLights_->lights_[j].decay = 1.0f;
							particles[i].pointLightIndex_ = static_cast<uint32_t>(j);
							particles[i].color_ = pointLights_->lights_[j].color;
							break;
						}
					}
					break;
				}
			}
		}
		isEmit_ = false;
	}

	for (size_t i = 0; i < kParticleNum; i++) {
		float rotationSpeed = Radian(1.0f) * (float(i % 2) * 2.0f - 1.0f);
		if (particles[i].isActive_ == true) {
			particles[i].worldTransform_.quaternion_ *= MakeFromAngleAxis({ 1.0f,1.0f,1.0f }, rotationSpeed);
			particles[i].worldTransform_.translation_ += particles[i].velocity_;
			particles[i].velocity_ += acceleration_;
			if (particles[i].worldTransform_.translation_.y <= -0.3f) {
				particles[i].worldTransform_.translation_.y = 0;
				particles[i].velocity_.y = -particles[i].velocity_.y * boundNum_;
				if (particles[i].firstDrop_ == false) {
					particles[i].firstDrop_ = true;
					auto& index = floor_->waveIndexData_.index_;
					if (index.size() < WaveIndexData::kMaxInfluenceWaveNum) {
						index.emplace_back(wavePoints_->EmitPoint(particles[i].worldTransform_.translation_));
					}
				}
			}
			particles[i].worldTransform_.scale_ = particles[i].worldTransform_.scale_ - scaleSpeed_;
			particles[i].worldTransform_.Update();
			pointLights_->lights_[particles[i].pointLightIndex_].intensity = 3.0f * (particles[i].worldTransform_.scale_.x / emitterWorldTransform_.scale_.x);
			pointLights_->lights_[particles[i].pointLightIndex_].worldTransform.Update();
			if (particles[i].worldTransform_.scale_.x <= 0.05f) {
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
			data.worldInverseTranspose = Inverse(particles[i].worldTransform_.matWorld_);
			data.color = particles[i].color_;
			particle_->PushBackData(data);
		}
	}

	DrawManager::GetInstance()->DrawParticleModel(*particle_);
}