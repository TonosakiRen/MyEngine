#include "EnemyBullet.h"
#include "ModelManager.h"
#include "Floor.h"

Vector3 EnemyBullet::modelSize;
void EnemyBullet::Initialize(Vector3 position, Vector3 direction,uint32_t modelHandle)
{
	GameObject::Initialize(modelHandle);
	worldTransform_.translation_ = position;
	worldTransform_.Update();
	collider_.Initialize(&worldTransform_, "enemyBullet", modelHandle);
	direction_ = direction;
	modelSize = ModelManager::GetInstance()->GetModelSize(modelHandle_);
	isDead_ = false;
}

void EnemyBullet::Update()
{
	worldTransform_.translation_ += direction_ * speed_;
	worldTransform_.Update();
	collider_.AdjustmentScale();

	if (worldTransform_.translation_.x > Floor::kFloorHalfSize + modelSize.x / 2.0f || worldTransform_.translation_.x < -Floor::kFloorHalfSize - modelSize.x / 2.0f ||
		worldTransform_.translation_.z > Floor::kFloorHalfSize + modelSize.z / 2.0f || worldTransform_.translation_.z < -Floor::kFloorHalfSize - modelSize.z / 2.0f ) {
		isDead_ = true;
	}
}

void EnemyBullet::OnCollision()
{
	isDead_ = true;
}

void EnemyBullet::Draw()
{
	GameObject::Draw({ 0.7f,0.0f,0.0f,1.0f });
}
