#include "PlayerBulletManager.h"
#include "ModelManager.h"

void PlayerBulletManager::Initialize()
{
	playerBullets_.clear();
	modelHandle_ = ModelManager::Load("sphere.obj");
}

void PlayerBulletManager::Update()
{

	//敵更新
	for (const std::unique_ptr<PlayerBullet>& playerBullet : playerBullets_) {
		playerBullet->Update();
	}

	// デスフラグの立った敵を削除
	playerBullets_.remove_if([](std::unique_ptr<PlayerBullet>& playerBullet) {
		if (playerBullet->IsDead()) {
			return true;
		}
		return false;
	});
}

void PlayerBulletManager::PopPlayerBullet(Vector3 position,Vector3 direction)
{
	// 弾を生成、初期化
	std::unique_ptr<PlayerBullet> newPlayerBullet = std::make_unique<PlayerBullet>();
	// 弾初期化
	newPlayerBullet->Initialize(position, direction, modelHandle_);
	// 弾を登録する
	playerBullets_.push_back(std::move(newPlayerBullet));
}

void PlayerBulletManager::Draw()
{
	//敵更新
	for (const std::unique_ptr<PlayerBullet>& playerBullet : playerBullets_) {
		playerBullet->Draw();
	}
}
