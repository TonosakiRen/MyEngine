/**
 * @file SphereLights.cpp
 * @brief sphereLight
 */
#include "Particle/SphereLights.h"
#include "ImGuiManager.h"
#include "Light/LightManager.h"
#include "Draw/DrawManager.h"
#include "Model/ModelManager.h"
#include "Stage/Floor.h"
#include <string>


void SphereLights::Initialize()
{
	pointLights_ = LightManager::GetInstance()->pointLights_.get();
	particle_ = std::make_unique<ParticleModelData>(kSphereNum);
	particle_->Initialize();

	material_ = std::make_unique<Material>();
	material_->Initialize();
	material_->enableLighting_ = false;
	material_->Update();

	//pointlightの割り当て初期化
	for (int j = 0; j < kSphereNum; j++) {
		for (int i = 0; i < PointLights::lightNum; i++) {
			if (pointLights_->lights_[i].isActive == false) {
				activeNum_++;
				Vector4 color = HSVAtoRGBA(Rand(0.0f, 1.0f), 1.0f, 1.0f, 1.0f);;
				spheres_[j].isActive_ = true;
				spheres_[j].pointLightIndex_ = i;
				spheres_[j].worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
				spheres_[j].worldTransform_.translation_ = { Rand(-Floor::kFloorHalfSize, Floor::kFloorHalfSize),1.0f,Rand(-Floor::kFloorHalfSize, Floor::kFloorHalfSize) };
				spheres_[j].worldTransform_.Update();
				spheres_[j].color_ = color;
				spheres_[j].intensity = intensity_;
				pointLights_->lights_[i].isActive = true;
				pointLights_->lights_[i].worldTransform.translation_ = { 0.0f,0.0f,0.0f };
				pointLights_->lights_[i].worldTransform.SetParent(&spheres_[j].worldTransform_,false);
				pointLights_->lights_[i].decay = 1.0f;
				pointLights_->lights_[i].intensity = intensity_;
				pointLights_->lights_[i].radius = radius_;
				pointLights_->lights_[i].color = color;
				break;
			}
		}
	}
}

void SphereLights::Emit(const Vector3& position,const Vector4& color)
{
	//出現
	for (int j = 0; j < kSphereNum; j++) {
		if (spheres_[j].isActive_ == false) {
			for (int i = 0; i < PointLights::lightNum; i++) {
				if (pointLights_->lights_[i].isActive == false) {
					activeNum_++;
					spheres_[j].isActive_ = true;
					spheres_[j].pointLightIndex_ = i;
					spheres_[j].worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
					spheres_[j].worldTransform_.translation_ = position;
					spheres_[j].worldTransform_.Update();
					spheres_[j].color_ = color;
					spheres_[j].intensity = intensity_;
					pointLights_->lights_[i].isActive = true;
					pointLights_->lights_[i].worldTransform.translation_ = { 0.0f,0.0f,0.0f };
					pointLights_->lights_[i].worldTransform.SetParent(&spheres_[j].worldTransform_, false);
					pointLights_->lights_[i].decay = 1.0f;
					pointLights_->lights_[i].intensity = intensity_;
					pointLights_->lights_[i].radius = radius_;
					pointLights_->lights_[i].color = color;
					break;
				}
			}
			break;
		}
	}
}

void SphereLights::Update() {
	for (int i = 0; i < kSphereNum; i++) {
		if (spheres_[i].isActive_ == true) {
			
		}
	}
}

void SphereLights::Draw()
{

	for (size_t i = 0; i < kSphereNum; i++)
	{
		if (spheres_[i].isActive_) {
			ParticleModelData::Data data;
			data.matWorld = spheres_[i].worldTransform_.matWorld_;
			data.worldInverseTranspose = Inverse(spheres_[i].worldTransform_.matWorld_);
			data.color = spheres_[i].color_;
			particle_->PushBackData(data);
		}
	}

	Engine::DrawManager::GetInstance()->DrawParticleModel(*particle_, Engine::ModelManager::GetInstance()->Load("sphere.obj"), *material_);
}