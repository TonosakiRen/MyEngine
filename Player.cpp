#include "Player.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Easing.h"
void Player::Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight)
{
	GameObject::Initialize(name, viewProjection, directionalLight);
	input_ = Input::GetInstance();

	material_.enableLighting_ = false;
	worldTransform_.rotation_.y = Radian(90.0f);
	accelaration_ = { 0.0f,0.002f };
	modelParts_.Initialize("player_part");
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].Initialize();
		partsTransform_[i].SetParent(&worldTransform_);
	}
	//model位置初期化
	{
		partsTransform_[LeftArm].translation_.x = 1.510f;
		partsTransform_[RightArm].translation_.x = -1.510f;
		partsTransform_[LeftArm].scale_ = { 0.4f,0.4f, 0.4f };
		partsTransform_[RightArm].scale_ = { 0.4f,0.4f, 0.4f };

		partsTransform_[RightLeg].translation_.y = 1.470f - 2.79f;
		partsTransform_[LeftLeg].translation_.y = 1.470f - 2.79f;
		partsTransform_[RightLeg].translation_.x = 1.120f;
		partsTransform_[LeftLeg].translation_.x = -1.120f;

		partsTransform_[LeftLeg].scale_ = { 0.4f,0.4f, 0.4f };
		partsTransform_[RightLeg].scale_ = { 0.4f,0.4f, 0.4f };
	}
}

void Player::Update()
{
	ImGui::Begin("Player");
	ImGui::DragFloat("up", &runUpAnimation_, 0.01f);
	ImGui::End();

	partsTransform_[RightArm].rotation_.x = Easing::easing(animationT_, -0.6f, 0.6f, animationSpeed_,Easing::EasingMode::easeInSine,false);
	partsTransform_[LeftArm].rotation_.x = -partsTransform_[RightArm].rotation_.x;

	partsTransform_[RightLeg].rotation_.x = Easing::easing(animationT_, -0.4f, 0.4f, animationSpeed_, Easing::EasingMode::easeInSine,false);
	partsTransform_[LeftLeg].rotation_.x = -partsTransform_[RightLeg].rotation_.x;

	worldTransform_.translation_.y += Easing::easing(animationBodyT_, 0.0f, runUpAnimation_, animationSpeed_, Easing::EasingMode::easeInSine, false);

	animationT_ += animationSpeed_;
	animationBodyT_ += animationBodySpeed_;

	if (animationT_ >= 1.0f || animationT_ <= 0.0f)
	{
		animationSpeed_ *= -1.0f;
	}

	if (animationBodyT_ >= 1.0f )
	{
		animationBodyT_ = 0.0f;
		runUpAnimation_ *= -1.0f;
	}

	if(input_->PushKey(DIK_SPACE)) {
		worldTransform_.translation_.y;
	}
	worldTransform_.translation_.y = clamp(worldTransform_.translation_.y, 2.79f, 17.0f);
	worldTransform_.UpdateMatrix();
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}
}

void Player::Draw() {
	model_.Draw(worldTransform_, *viewProjection_, *directionalLight_, material_);
	for (int i = 0; i < partNum; i++) {
		modelParts_.Draw(partsTransform_[i], *viewProjection_, *directionalLight_, material_);
	}
}
