/**
 * @file EnemyBulletManager.cpp
 * @brief 敵弾の管理
 */
#include "Enemy/EnemyBulletManager.h"
#include "Model/ModelManager.h"

void EnemyBulletManager::Initialize(ExplodeParticle* explodeParticle)
{
	explodeParticle_ = explodeParticle;
	enemyBullets_.clear();
	modelHandle_ = Engine::ModelManager::Load("sphere.obj");
}

void EnemyBulletManager::Update()
{

	//敵更新
	for (const std::unique_ptr<EnemyBullet>& enemyBullet : enemyBullets_) {
		enemyBullet->Update();
	}

	// デスフラグの立った敵を削除
	enemyBullets_.remove_if([&](std::unique_ptr<EnemyBullet>& enemyBullet) {
		if (enemyBullet->IsDead()) {
			if (enemyBullet->GetWorldTransform()->translation_.y >= 0.0f) {
				//explodeParticle_->SetIsEmit(true, MakeTranslation(enemyBullet->GetWorldTransform()->matWorld_));
			}
			return true;
		}
		return false;
	});
}

void EnemyBulletManager::PopEnemyBullet(Vector3 position, Vector3 direction)
{
	// 敵弾を生成、初期化
	std::unique_ptr<EnemyBullet> newEnemyBullet = std::make_unique<EnemyBullet>();
	// 敵弾初期化
	newEnemyBullet->Initialize(position, direction, modelHandle_);
	// 敵弾を登録する
	enemyBullets_.push_back(std::move(newEnemyBullet));
}

void EnemyBulletManager::Draw()
{
	//敵更新
	for (const std::unique_ptr<EnemyBullet>& enemyBullet : enemyBullets_) {
		enemyBullet->Draw();
	}
}
