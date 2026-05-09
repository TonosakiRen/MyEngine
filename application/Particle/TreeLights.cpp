/**
 * @file TreeLights.cpp
 * @brief sphereLight
 */
#include "Particle/TreeLights.h"
#include "ImGuiManager.h"
#include "Light/LightManager.h"
#include "Draw/DrawManager.h"
#include "Model/ModelManager.h"
#include "Stage/Floor.h"
#include "Stage/Trees.h"
#include <string>


void TreeLights::Initialize()
{
	pointLights_ = LightManager::GetInstance()->pointLights_.get();
	particle_ = std::make_unique<ParticleModelData>(kSphereNum);
	particle_->Initialize();

	material_ = std::make_unique<Material>();
	material_->Initialize();
	material_->enableLighting_ = false;
	material_->Update();

	const float offset = Floor::kFloorHalfSize + 3.0f;

	//pointlightの割り当て初期化
	for (int j = 0; j < kSphereNum; j++) {
		for (int i = 0; i < PointLights::lightNum; i++) {
			if (pointLights_->lights_[i].isActive == false) {

				int remainder = j % 4;
				Vector3 position;
				switch (remainder)
				{
				case 0:
					position = { offset ,15.0f + Rand(2.0f,-5.0f),Rand(-offset,offset)};
					break;
				case 1:
					position = { -offset ,15.0f + Rand(2.0f,-5.0f),Rand(-offset,offset) };
					break;
				case 2:
					position = { Rand(-offset,offset)  ,15.0f + Rand(2.0f,-5.0f), offset };
					break;
				case 3:
					position = { Rand(-offset,offset),15.0f + Rand(2.0f,-5.0f), -offset };
					break;
				default:
					break;
				}

				activeNum_++;
				Vector4 color = HSVAtoRGBA(Rand(0.0f, 1.0f), 1.0f, 1.0f, 1.0f);;
				spheres_[j].isActive_ = true;
				spheres_[j].pointLightIndex_ = i;
				spheres_[j].worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
				spheres_[j].worldTransform_.translation_ = position;
				spheres_[j].worldTransform_.Update();
				spheres_[j].color_ = color;
				spheres_[j].intensity = intensity_;
				pointLights_->lights_[i].isActive = true;
				pointLights_->lights_[i].worldTransform.translation_ = { 0.0f,0.0f,0.0f };
				pointLights_->lights_[i].worldTransform.SetParent(&spheres_[j].worldTransform_,false);
				pointLights_->lights_[i].decay = 2.0f;
				pointLights_->lights_[i].intensity = intensity_;
				pointLights_->lights_[i].radius = radius_;
				pointLights_->lights_[i].color = color;
				break;
			}
		}
	}
}

void TreeLights::Emit(const Vector3& position,const Vector4& color)
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

void TreeLights::Update() {
	for (int i = 0; i < kSphereNum; i++) {
		if (spheres_[i].isActive_ == true) {
			
		}
	}
}