#include "Stage/LightObjects.h"
#include "Draw/DrawManager.h"
#include "Stage/Floor.h"
#include "ImGuiManager.h"

void LightObjects::Initialize(const std::string& name)
{
	pointLights_ = LightManager::GetInstance()->pointLights_.get();
	material_ = std::make_unique<Material>();
	material_->Initialize();
	//material_->enableLighting_ = false;
	material_->intensity_ = 2.0f;
	material_->Update();
	data_ = std::make_unique<ParticleModelData>(kObjectNum_);
	data_->Initialize();
	color_ = HSVAtoRGBA(0.5f, 1.0f, 1.0f, 1.0f);
	for (auto& ball : objects_) {
		Vector3 position = { Rand(Vector3{-Floor::kFloorHalfSize,0.0f,Floor::kFloorHalfSize},Vector3{Floor::kFloorHalfSize,0.0f,Floor::kFloorSize}) };
		for (int i = 0; i < PointLights::lightNum; i++) {
			if (pointLights_->lights_[i].isActive == false) {
				ball.Initialize(name, &pointLights_->lights_[i], color_, {1.5f,1.5f,1.5f}, IdentityQuaternion(), position );
				pointLights_->lights_[i].isActive = true;
				pointLights_->lights_[i].worldTransform.Reset();
				pointLights_->lights_[i].worldTransform.translation_ = { 0.0f,0.0f,0.0f };
				pointLights_->lights_[i].worldTransform.SetParent(ball.GetWorldTransform(), false);
				pointLights_->lights_[i].decay = 1.0f;
				pointLights_->lights_[i].intensity = 20.0f;
				pointLights_->lights_[i].radius = 6.0f;
				pointLights_->lights_[i].color = color_;
				break;
			}
		}
	}
}

void LightObjects::Update()
{
	
	for (int i = 0; i < kObjectNum_; i++) {
		objects_[i].SetColor(color_);
		objects_[i].Update();
	}
}

void LightObjects::Draw()
{
	for (size_t i = 0; i < kObjectNum_; i++)
	{
		if (objects_[i].isActive_) {
			ParticleModelData::Data data;
			data.matWorld = objects_[i].worldTransform_.matWorld_;
			data.worldInverseTranspose = Inverse(objects_[i].worldTransform_.matWorld_);
			data.color = objects_[i].material_.color_;
			data_->PushBackData(data);
		}
	}

	DrawManager::GetInstance()->DrawParticleModel(*data_, ModelManager::GetInstance()->Load("sphere.obj"), *material_);
}
