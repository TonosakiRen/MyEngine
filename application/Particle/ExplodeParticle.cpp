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
	const float emitScale = 0.8f;
	emitterWorldTransform_.scale_ = { emitScale,emitScale,emitScale };
	emitterWorldTransform_.Update();
	modelHandle_ = Engine::ModelManager::GetInstance()->Load("dodecahedron.obj");
}

void ExplodeParticle::Update() {

	emitterWorldTransform_.Update();
	const float radius = 3.0f;
	const float intensity = 3.0f;
	
	//出現
	if (isEmit_) {
		for (size_t i = 0; i < EmitNum_; i++) {
			for (size_t k = 0; k < kParticleNum; k++) {
				if (particles[k].isActive_ == false) {
					particles[k].isActive_ = true;
					particles[k].velocity_ = { Rand(-directionScalar_,directionScalar_),Rand(initVelocity_.x,initVelocity_.y), Rand(-directionScalar_,directionScalar_) };
					particles[k].worldTransform_.translation_ = MakeTranslation(emitterWorldTransform_.matWorld_);
					particles[k].worldTransform_.quaternion_ = IdentityQuaternion();
					particles[k].worldTransform_.scale_ = emitterWorldTransform_.scale_;
					particles[k].worldTransform_.Update();
					particles[k].firstDrop_ = false;
					for (size_t j = 0; j < PointLights::lightNum; j++) {
						if (pointLights_->lights_[j].isActive == false) {
							pointLights_->lights_[j].worldTransform.Reset();
							pointLights_->lights_[j].worldTransform.translation_ = {0.0f,0.0f,0.0f};
							pointLights_->lights_[j].worldTransform.SetParent(&particles[k].worldTransform_,false);
							pointLights_->lights_[j].isActive = true;
							pointLights_->lights_[j].color = emitColor_;
							pointLights_->lights_[j].radius = radius;
							pointLights_->lights_[j].intensity = intensity;
							pointLights_->lights_[j].decay = 1.0f;
							particles[k].pointLight_ = &pointLights_->lights_[j];
							particles[k].color_ = emitColor_;
							break;
						}
					}
					break;
				}
			}
		}
		isEmit_ = false;
	}

	//更新
	const float minScale = 0.005f;
	for (size_t i = 0; i < kParticleNum; i++) {
		//ランダムで+か-か
		float rotationSpeed = Radian(1.0f) * (float(i % 2) * 2.0f - 1.0f);
		if (particles[i].isActive_ == true) {
			//移動回転
			particles[i].worldTransform_.quaternion_ *= MakeFromAngleAxis({ 1.0f,1.0f,1.0f }, rotationSpeed);
			particles[i].worldTransform_.translation_ += particles[i].velocity_;
			particles[i].velocity_ += acceleration_;
			//落ちたら
			if (particles[i].worldTransform_.translation_.y <= -0.3f) {
				particles[i].worldTransform_.translation_.y = 0;
				particles[i].velocity_.y = -particles[i].velocity_.y * boundNum_;
				//最初の落下だとしてら波を発生させ波インデックスに登録
				if (particles[i].firstDrop_ == false) {
					particles[i].firstDrop_ = true;
					auto& index = floor_->waveIndexData_.GetIndex();
					if (index.size() < WaveIndexData::kMaxInfluenceWaveNum) {
						index.emplace_back(wavePoints_->EmitPoint(particles[i].worldTransform_.translation_));
					}
				}
			}
			//だんだんscaleを小さく
			particles[i].worldTransform_.scale_ = particles[i].worldTransform_.scale_ - scaleSpeed_;
			particles[i].worldTransform_.Update();
			particles[i].pointLight_->intensity = intensity * (particles[i].worldTransform_.scale_.x / emitterWorldTransform_.scale_.x);
			//小さくなったら
			if (particles[i].worldTransform_.scale_.x <= minScale) {
				particles[i].isActive_ = false;
				particles[i].pointLight_->isActive = false;
			}
		}

	}

}

void ExplodeParticle::Draw()
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

	Engine::DrawManager::GetInstance()->DrawParticleModel(*particle_, modelHandle_);
}