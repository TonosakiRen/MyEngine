#include "Rain/RainDrop.h"
#include "Model/ModelManager.h"
#include "Stage/Floor.h"

Vector3 RainDrop::modelSize;

void RainDrop::Initialize(uint32_t modelHandle,PointLight* pointLight, ExplodeParticle* explodeParticle,const Vector3& position,const Vector4& color)
{
	GameObject::Initialize(modelHandle);
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	worldTransform_.Update();

	collider_.Initialize(&worldTransform_, "rainDrop", modelHandle);
	acceleration_ = { 0.0f,-0.01f,0.0f };
	modelSize = ModelManager::GetInstance()->GetModelSize(modelHandle_);
	color_ = color;

	pointLight->isActive = true;
	pointLight->worldTransform.Reset();
	pointLight->worldTransform.translation_ = { 0.0f,0.0f,0.0f };
	pointLight->worldTransform.SetParent(&worldTransform_, false);
	pointLight->decay = 1.0f;
	pointLight->intensity = 3.0f;
	pointLight->radius = 6.0f;
	pointLight->color = color;

	pointLight_ = pointLight;
	explodeParticle_ = explodeParticle;
	isActive_ = true;
	isDrop_ = false;
	dropFrame_ = anActiveFrame_;
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
}

void RainDrop::Finalize()
{
	isActive_ = false;
	pointLight_->isActive = false;
	isDrop_ = false;
	dropFrame_ = anActiveFrame_;
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
}

void RainDrop::Update()
{

	velocity_ += acceleration_;
	worldTransform_.translation_ += velocity_;

	if (worldTransform_.translation_.y < 0.0f) {
		
		if (!isDrop_) {
			isDrop_ = true;
			explodeParticle_->SetIsEmit(true, MakeTranslation(worldTransform_.matWorld_),color_);
		}
		worldTransform_.translation_.y = 0.0f;
		velocity_.y = 0.0f;
	}

	if (isDrop_) {
		dropFrame_--;
		float t = float(dropFrame_) / float(anActiveFrame_);
		pointLight_->intensity = 3.0f * t;
		worldTransform_.scale_ = {1.0f * t,1.0f * t,1.0f * t};
	}

	if (dropFrame_ == 0) {
		Finalize();
	}

	worldTransform_.Update();
	collider_.Draw();
}

void RainDrop::OnCollision()
{
	Finalize();
}

