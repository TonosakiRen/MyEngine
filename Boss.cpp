#include "Boss.h"
#include "ImGuiManager.h"
#include "Easing.h"
#include "Player.h"
#include "GameScene.h"

Boss::Boss(ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector3 pos)
{
	GameObject::Initialize(viewProjection, directionalLight);
	worldTransform_.translation_ = pos;
	initialPos_ = pos;
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

	collider_.Initialize(&worldTransform_, "boss", viewProjection, directionalLight, { 1.8f,7.3f,2.2f }, { 0.0f,-5.4f,0.9f });
}


void Boss::Update()
{

	Animation();

	worldTransform_.translation_.y = clamp(worldTransform_.translation_.y, 10.0f, 100.0f);

	speedDiff += std::fabsf(speed_);
	if (speedDiff >= 2.0f) {
		speedDiff = 0.0f;
		speed_ *= -1.0f;
	}
	worldTransform_.translation_.z += speed_;

	if (isDead_ == true) {
		alpha_ -= 0.1f;
		worldTransform_.translation_.y += 0.3f;
	}


	//行列更新
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}
	worldTransform_.UpdateMatrix();

}
void Boss::Animation() {

	//jump
	if (isDead_ == false) {
		animationVelocity_ += animationAccelaration_;
		if (worldTransform_.translation_.y <= 10.0f) {
			animationVelocity_.y = jumpPower_;
		}
		worldTransform_.translation_ += animationVelocity_;
	}

	//上の頭アニメーション
	partsTransform_[Head].translation_.y = Easing::easing((worldTransform_.translation_.y - 10.0f) / 3.9f, -3.0f, 0.8f);


	partsTransform_[Head].translation_.y = clamp(partsTransform_[Head].translation_.y, -3.0f, 0.8f);
	partsTransform_[Tin].translation_.y = clamp(partsTransform_[Tin].translation_.y, -6.0f, -2.2f);
}
void Boss::OnCollision()
{
	hp_--;
	if (hp_ <= 0) {
		isDead_ = true;
	}
}
void Boss::Respowan()
{
	isDead_ = false;
	alpha_ = 1.0f;
	hp_ = 3;
	worldTransform_.translation_ = initialPos_;
}
void Boss::Draw() {
	material_.color_.w = alpha_;
	material_.UpdateMaterial();
	
	for (int i = 0; i < partNum; i++) {
		modelParts_[i].Draw(partsTransform_[i], *viewProjection_, *directionalLight_, material_);
	}

}
