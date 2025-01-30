/**
 * @file Enemy.cpp
 * @brief 敵
 */
#include "Enemy/Enemy.h"
#include "Enemy/EnemyBulletManager.h"
#include "Player/Player.h"
#include "Model/ModelManager.h"
#include "Stage/Floor.h"

uint32_t Enemy::deadEnemyNum = 0;
Vector3 Enemy::modelSize;

void Enemy::Initialize(uint32_t modelHandle,Vector3 position, EnemyBulletManager* enemyBulletManager, Player* player,Vector3 direction)
{
	GameObject::Initialize(modelHandle);
	worldTransform_.translation_ = position;
	worldTransform_.Update();
	collider_.Initialize(&worldTransform_,"enemy", modelHandle);
	direction_ = direction;
	modelSize = Engine::ModelManager::GetInstance()->GetModelSize(modelHandle_);
	enemyBulletManager_ = enemyBulletManager;
	player_ = player;
}

void Enemy::Update()
{

	worldTransform_.translation_ += direction_ * speed_;

	const uint32_t fireInterval = 150;
	if (fireFrame_ <= 0) {
		fireFrame_ = fireInterval;
		Fire();
	}
	fireFrame_--;

	if (worldTransform_.translation_.x > Floor::kFloorHalfSize + modelSize.x / 2.0f  || worldTransform_.translation_.x < -Floor::kFloorHalfSize - modelSize.x / 2.0f ||
		worldTransform_.translation_.z > Floor::kFloorHalfSize + modelSize.z / 2.0f || worldTransform_.translation_.z < -Floor::kFloorHalfSize - modelSize.z / 2.0f
		) {
		isDead_ = true;
	}

	worldTransform_.Update();
}

void Enemy::OnCollision()
{
	isDead_ = true;
	deadEnemyNum++;
}

void Enemy::Draw()
{
	const Vector4 color = { 0.4f,0.4f,0.4f,1.0f };
	GameObject::Draw(color);
}

void Enemy::Fire()
{
	Vector3 position = MakeTranslation(worldTransform_.matWorld_);
	Vector3 playerPos = MakeTranslation(player_->GetWorldTransform().matWorld_);
	Vector3 direction = Normalize(playerPos - position);
	enemyBulletManager_->PopEnemyBullet(position,direction);
}

