#include "PlayerBullet.h"
#include "ModelManager.h"
#include "Floor.h"
Vector3 PlayerBullet::modelSize;
void PlayerBullet::Initialize(Vector3 position, Vector3 direction,uint32_t modelHandle)
{
	GameObject::Initialize(modelHandle);
	worldTransform_.translation_ = position;
	worldTransform_.Update();
	collider_.Initialize(&worldTransform_, "playerBullet", modelHandle);

	modelSize = ModelManager::GetInstance()->GetModelSize(modelHandle_);
	direction_ = direction;
}

void PlayerBullet::Update()
{
	worldTransform_.translation_ += direction_ * speed_;
	worldTransform_.Update();
	collider_.AdjustmentScale();

	if (worldTransform_.translation_.x > Floor::kFloorHalfSize + modelSize.x / 2.0f || worldTransform_.translation_.x < -Floor::kFloorHalfSize - modelSize.x / 2.0f ||
		worldTransform_.translation_.z > Floor::kFloorHalfSize + modelSize.z / 2.0f || worldTransform_.translation_.z < -Floor::kFloorHalfSize - modelSize.z / 2.0f
		) {
		isDead_ = true;
	}
}

void PlayerBullet::OnCollision()
{
	isDead_ = true;
}

void PlayerBullet::Draw()
{
	GameObject::Draw({0.0f,0.0f,1.0f,1.0f});
}
