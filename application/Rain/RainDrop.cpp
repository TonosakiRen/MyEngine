/**
 * @file RainDrop.cpp
 * @brief 雨粒
 */
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

	//ポイントライト初期化
	color_ = color;
	pointLight->isActive = true;
	pointLight->worldTransform.Reset();
	pointLight->worldTransform.translation_ = { 0.0f,0.0f,0.0f };
	pointLight->worldTransform.SetParent(&worldTransform_, false);
	pointLight->decay = 1.0f;
	pointLight->intensity = intensity_;
	pointLight->radius = radius_;
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
	//落下
	velocity_ += acceleration_;
	worldTransform_.translation_ += velocity_;

	//落下したら埋まる、パーティクル出現
	if (worldTransform_.translation_.y < 0.0f) {
		
		if (!isDrop_) {
			isDrop_ = true;
			explodeParticle_->SetIsEmit(true, MakeTranslation(worldTransform_.matWorld_),color_);
		}
		worldTransform_.translation_.y = 0.0f;
		velocity_.y = 0.0f;
	}

	//徐々に小さく
	if (isDrop_) {
		dropFrame_--;
		float t = float(dropFrame_) / float(anActiveFrame_);
		pointLight_->intensity = intensity_ * t;
		worldTransform_.scale_ = {1.0f * t,1.0f * t,1.0f * t};
	}

	//消える
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

