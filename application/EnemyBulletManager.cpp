#include "EnemyBulletManager.h"
#include "ModelManager.h"

void EnemyBulletManager::Initialize()
{
	enemyBullets_.clear();
	modelHandle_ = ModelManager::Load("sphere");
}

void EnemyBulletManager::Update()
{

	//敵更新
	for (const std::unique_ptr<EnemyBullet>& enemyBullet : enemyBullets_) {
		enemyBullet->Update();
	}

	// デスフラグの立った敵を削除
	enemyBullets_.remove_if([](std::unique_ptr<EnemyBullet>& enemyBullet) {
		if (enemyBullet->IsDead()) {
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
