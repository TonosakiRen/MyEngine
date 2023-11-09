#include "Boss.h"
#include "ImGuiManager.h"
#include "Easing.h"
#include "Player.h"
#include "GameScene.h"

void Boss::Initialize(ViewProjection* viewProjection, DirectionalLight* directionalLight)
{
	GameObject::Initialize(viewProjection, directionalLight);

	dustParticle_ = std::make_unique<DustParticle>();
	dustParticle_->Initialize({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
	dustParticle_->emitterWorldTransform_.SetParent(&worldTransform_);
	//煙の出る場所
	dustParticle_->emitterWorldTransform_.translation_ = { 0.0f,-2.1f,-1.2f };

	worldTransform_.translation_.y = 10.0f;
	worldTransform_.translation_.x = -20.0f;

	worldTransform_.quaternion_ *= MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, Radian(90.0f));
	modelParts_[Head].Initialize("boss_head");
	modelParts_[Tin].Initialize("boss_tin");
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].Initialize();
		partsTransform_[i].SetParent(&worldTransform_);
	}
	//model位置初期化
	{
		partsTransform_[Head].translation_.y = -3.0f;
		partsTransform_[Tin].translation_.y = -6.0f;
	}

	collider_.Initialize(&worldTransform_,"boss",viewProjection,directionalLight,{1.8f,7.3f,2.2f},{0.0f,-5.4f,0.9f});
}

void Boss::Update()
{

	Animation();
	

	worldTransform_.translation_.y = clamp(worldTransform_.translation_.y, 10.0f, 13.9f);


	//行列更新
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}
	worldTransform_.UpdateMatrix();
	dustParticle_->Update();

}
void Boss::Animation() {

	//jump
	animationVelocity_ += animationAccelaration_;
	if (worldTransform_.translation_.y <= 10.0f) {
		animationVelocity_.y = jumpPower_;
	}
	worldTransform_.translation_ += animationVelocity_;


	//上の頭アニメーション
	partsTransform_[Head].translation_.y = Easing::easing((worldTransform_.translation_.y - 10.0f) / 3.9f, -3.0f, 0.8f);


	partsTransform_[Head].translation_.y = clamp(partsTransform_[Head].translation_.y, -3.0f, 0.8f);
	partsTransform_[Tin].translation_.y = clamp(partsTransform_[Tin].translation_.y, -6.0f, -2.2f);
}
void Boss::OnCollision()
{
	isDead_ = true;
}
void Boss::Draw() {
	if (isDead_ == false) {
		for (int i = 0; i < partNum; i++) {
			modelParts_[i].Draw(partsTransform_[i], *viewProjection_, *directionalLight_, material_);
		}
	}
}

void Boss::ParticleDraw() {
	dustParticle_->Draw(viewProjection_, directionalLight_, { 0.5f,0.5f,0.5f,1.0f });
}

