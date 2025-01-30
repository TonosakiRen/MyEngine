/**
 * @file Mushrooms.cpp
 * @brief キノコ達
 */
#include "Stage/Mushrooms.h"
#include "Draw/DrawManager.h"
#include "Stage/Floor.h"
#include "ImGuiManager.h"
#include "GameComponent/Loader.h"
#include "Texture/TextureManager.h"

Mushrooms::Mushrooms()
{
	//particleData_ = std::make_unique<ParticleData>(kMushroomParticleNum);
}

void Mushrooms::Initialize(const std::string& name,WorldTransform& parentWorldTransform)
{
	
	pointLights_ = LightManager::GetInstance()->pointLights_.get();
	modelHandle_ = Engine::ModelManager::Load(name);
	material_ = std::make_unique<Material>();
	material_->Initialize();
	material_->intensity_ = 2.0f;
	material_->Update();
	data_ = std::make_unique<ParticleModelData>(kMushroomNum_);
	data_->Initialize();
	//particleData_->Initialize();

	particleTextureHandle_ = Engine::TextureManager::GetInstance()->Load("particle.png");

	const Vector4 initColor = { 0.2f,0.2f,1.0f,1.0f };
	const float intensity = 20.0f;
	const float radius = 6.0f;

	color_ = initColor;

	std::vector<Transform> transforms;
	Load::Transforms("mushroom.txt", transforms);

	//キノコの初期化
	for (int mushroomNum = 0; mushroomNum < kMushroomNum_; mushroomNum++) {
		Vector3 position = { Rand(Vector3{-Floor::kFloorHalfSize,0.0f,Floor::kFloorHalfSize},Vector3{Floor::kFloorHalfSize,0.0f,Floor::kFloorSize}) };
		float intensityT_ = Rand(0.0f, 1.0f);
		for (int i = 0; i < PointLights::lightNum; i++) {
			if (pointLights_->lights_[i].isActive == false) {
				mushrooms[mushroomNum].Initialize(name, &parentWorldTransform,&pointLights_->lights_[i], color_, intensityT_, transforms[mushroomNum].scale,
				MakeFromEulerAngle(Vector3{ Radian(transforms[mushroomNum].rotate.x),Radian(transforms[mushroomNum].rotate.y),Radian(transforms[mushroomNum].rotate.z) }),
				transforms[mushroomNum].translate);
				pointLights_->lights_[i].isActive = true;
				pointLights_->lights_[i].worldTransform.Reset();
				pointLights_->lights_[i].worldTransform.SetParent(mushrooms[mushroomNum].GetWorldTransform(), false);
				pointLights_->lights_[i].decay = 1.0f;
				pointLights_->lights_[i].intensity = intensity;
				pointLights_->lights_[i].radius = radius;
				pointLights_->lights_[i].color = color_;
				break;
			}
		}
	}
}

void Mushrooms::ChangeColorSphere(Collider& collider, const Vector4& color)
{
	for (size_t i = 0; i < kMushroomNum_; i++)
	{
		if (!mushrooms[i].isChangeColor_) {
			if (mushrooms[i].collider_.SphereCollision(collider)) {
				mushrooms[i].SetColor(color);
			}
		}
	}
}

void Mushrooms::Update()
{
	
	material_->Update();

	for (int i = 0; i < kMushroomNum_; i++) {
		mushrooms[i].Update();
	}
}

void Mushrooms::Draw()
{
	for (size_t i = 0; i < kMushroomNum_; i++)
	{
		if (mushrooms[i].isActive_) {
			ParticleModelData::Data data;
			data.matWorld = mushrooms[i].worldTransform_.matWorld_;
			data.worldInverseTranspose = Inverse(mushrooms[i].worldTransform_.matWorld_);
			data.color = mushrooms[i].material_.color_;
			data_->PushBackData(data);
		}

		//mushrooms[i].PushDataParticle(particleData_);

		//DrawManager::GetInstance()->DrawParticle(*particleData_, particleTextureHandle_, mushrooms[i].material_);
	}

	Engine::DrawManager::GetInstance()->DrawParticleModel(*data_, modelHandle_, *material_);
}
