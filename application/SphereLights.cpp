#include "SphereLights.h"
#include "ImGuiManager.h"
#include "LightManager.h"
#include "DrawManager.h"
#include "ModelManager.h"
#include <string>

SphereLights::SphereLights()
{
	particle_ = std::make_unique<ParticleModelData>(kSphereNum);
}

void SphereLights::Initialize()
{
	pointLights_ = LightManager::GetInstance()->pointLights_.get();
	particle_->Initialize();

	spheres_.resize(kSphereNum);
	for (int j = 0; j < kSphereNum; j++) {
		for (int i = 0; i < PointLights::lightNum; i++) {
			if (pointLights_->lights_[i].isActive == false) {
				activeNum_++;
				spheres_[j].isActive_ = true;
				spheres_[j].pointLightIndex_ = i;
				spheres_[j].worldTransform_.scale_ = { 0.1f,0.1f,0.1f };
				spheres_[j].worldTransform_.translation_ = { Rand(-20.0f, 20.0f),0.5f,Rand(-20.0f, 20.0f) };
				spheres_[j].worldTransform_.Update();
				pointLights_->lights_[i].isActive = true;
				pointLights_->lights_[i].worldTransform.translation_ = { 0.0f,0.0f,0.0f };
				pointLights_->lights_[i].worldTransform.SetParent(&spheres_[j].worldTransform_,false);
				pointLights_->lights_[i].decay = 1.0f;
				pointLights_->lights_[i].intensity = 3.0f;
				pointLights_->lights_[i].radius = 3.0f;
				pointLights_->lights_[i].color = HSVA(Rand(0.0f, 1.0f), 1.0f, 1.0f, 1.0f);
				break;
			}
		}
	}
}

void SphereLights::Update() {
#ifdef USE_IMGUI
	ImGui::Begin("Game");
	if (ImGui::BeginMenu("SphereLight")) {
		/*if (ImGui::Button("spawn") && activeNum_ < kSphereNum) {
			for (int j = 0; j < kSphereNum; j++) {
				if (spheres_[j].isActive_ == false) {
					for (int i = 0; i < PointLights::lightNum; i++) {
						if (pointLights_->lights_[i].isActive == false) {
							activeNum_++;
							spheres_[j].isActive_ = true;
							spheres_[j].pointLightIndex_ = i;
							spheres_[j].worldTransform_.scale_ = { 0.1f,0.1f,0.1f };
							pointLights_->lights_[i].isActive = true;
							pointLights_->lights_[i].worldTransform.translation_ = { 0.0f,0.0f,0.0f };
							pointLights_->lights_[i].worldTransform.parent_ = &spheres_[j].worldTransform_;
							pointLights_->lights_[i].decay = 1.0f;
							pointLights_->lights_[i].intensity = 3.0f;
							pointLights_->lights_[i].radius = 3.0f;
							pointLights_->lights_[i].color = HSVA(Rand(0.0f, 1.0f), 1.0f, 1.0f, 1.0f);
							break;
						}
					}
					break;
				}
			}
		}*/

		ImGui::Text("activeNum : %d", activeNum_);
		for (int i = 0; auto & sphere : spheres_) {
			std::string name = { "sphere" };
			if (sphere.isActive_) {
				name += std::to_string(i);
				ImGui::DragFloat3(name.c_str(), &sphere.worldTransform_.translation_.x, 0.1f);
				sphere.worldTransform_.Update();
			}
			i++;
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif

	pointLights_->Update();
}

void SphereLights::Draw(Vector4 color)
{

	for (size_t i = 0; i < kSphereNum; i++)
	{
		if (spheres_[i].isActive_) {
			ParticleModelData::Data data;
			data.matWorld = spheres_[i].worldTransform_.matWorld_;
			data.worldInverseTranspose = Inverse(spheres_[i].worldTransform_.matWorld_);
			particle_->PushBackData(data);
		}
	}

	DrawManager::GetInstance()->DrawParticleModel(*particle_, ModelManager::GetInstance()->Load("sphere.obj"));
}