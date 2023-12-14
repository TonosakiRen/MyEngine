#include "Player.h"
#include "ImGuiManager.h"
void Player::Initialize(const std::string name)
{
	GameObject::Initialize(name);
	input_ = Input::GetInstance();
	collider_.Initialize(&worldTransform_,name);
	worldTransform_.translation_ = { 0.0f,2.0f,0.0f };
	velocisity_ = { 0.0f,0.0f,0.0f };
	acceleration_ = { 0.0f,-0.05f,0.0f };
}

void Player::Update()
{

	Vector3 move = {0.0f,0.0f,0.0f};

	if (input_->PushKey(DIK_A)) {
		move.x -= 0.3f;
	}
	if (input_->PushKey(DIK_D)) {
		move.x += 0.3f;
	}
	if (input_->PushKey(DIK_S)) {
		move.z -= 0.3f;
	}
	if (input_->PushKey(DIK_W)) {
		move.z += 0.3f;
	}

	if (input_->TriggerKey(DIK_SPACE)) {
		velocisity_.y = 1.0f;
	}

	worldTransform_.translation_ += move;

	ImGui::Begin("Player");
	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("sca", &worldTransform_.scale_.x, 0.1f);
	ImGui::End();

	collider_.AdjustmentScale();
	velocisity_.y = clamp(velocisity_.y, -0.5f, 200.0f);
	velocisity_ += acceleration_;
	worldTransform_.translation_ += velocisity_;
	worldTransform_.Update();
}

void Player::Collision(Collider& otherCollider)
{
	Vector3 pushBackVector;
	if (collider_.Collision(otherCollider, pushBackVector)) {
		worldTransform_.translation_ += pushBackVector;
		worldTransform_.Update();
	}

}

void Player::Draw() {
	collider_.Draw();
	GameObject::Draw({1.0f,1.0f,1.0f});
}