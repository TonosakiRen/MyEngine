#include "GamePlayScene.h"
#include "GameScene.h"

#include "ModelManager.h"
#include "ImGuiManager.h"

const Player* GamePlayScene::player = nullptr;

void GamePlayScene::Initialize()
{
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize("skydome");

	floor_ = std::make_unique<Floor>();
	floor_->Initialize("floor");

	sphere_ = std::make_unique<GameObject>();
	sphere_->Initialize("sphere");
	sphere_->SetPosition({ 0.0f,8.0f,-3.0f });
	sphere_->UpdateMatrix();

	boxArea_ = std::make_unique<BoxArea>();
	boxArea_->Initialize("box1x1Inverse");

	explodeParticle_ = std::make_unique<ExplodeParticle>();
	explodeParticle_->Initialize(Vector3{ -1.0f,-1.0f,-1.0f }, Vector3{ 1.0f,1.0f,1.0f }, GameScene::pointLights);

	whiteParticle_ = std::make_unique<WhiteParticle>();
	whiteParticle_->SetIsEmit(true);
	whiteParticle_->Initialize(Vector3{ -1.0f,-1.0f,-1.0f }, Vector3{ 1.0f,1.0f,1.0f });

	enemyBulletManager_ = std::make_unique<EnemyBulletManager>();
	playerBulletManager_ = std::make_unique<PlayerBulletManager>();
	enemyBulletManager_->Initialize();
	playerBulletManager_->Initialize();

	player_ = std::make_unique<Player>();
	player_->Initialize("sphere", playerBulletManager_.get());
	player = player_.get();
}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::Update()
{
	boxArea_->Update();

	player_->Update(*GameScene::currentViewProjection_);

	//敵更新
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Update();
	}

	// デスフラグの立った敵を削除
	enemies_.remove_if([](std::unique_ptr<Enemy>& enemy) {
		if (enemy->IsDead()) {
			return true;
		}
		return false;
		});

	//敵弾更新
	enemyBulletManager_->Update();

	//敵弾更新
	playerBulletManager_->Update();

	//当たり判定
	CheckAllCollision();

	explodeParticle_->Update();

	//敵出現
	const int spawnInterval = 300;
	if (enemySpawnFrame_ <= 0) {
		enemySpawnFrame_ = spawnInterval;
		EnemySpawn({ 0.0f,3.0f,0.0f });
	}
	enemySpawnFrame_--;


#ifdef _DEBUG
	//パーティクル
	ImGui::Begin("particle");
	ImGui::DragFloat3("emitterPos", &whiteParticle_->emitterWorldTransform_.translation_.x, 0.01f);
	ImGui::End();
#endif
	whiteParticle_->Update();
}

void GamePlayScene::ModelDraw()
{
	boxArea_->Draw();
	player_->Draw();
	sphere_->Draw();
	//敵描画
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Draw();
	}

	enemyBulletManager_->Draw();
	playerBulletManager_->Draw();
}

void GamePlayScene::ShadowDraw()
{
	player_->Draw();
	sphere_->Draw();
}

void GamePlayScene::SpotLightShadowDraw()
{
	player_->Draw();
	sphere_->Draw();
}

void GamePlayScene::ParticleDraw()
{
	whiteParticle_->Draw({ 1.0f,1.0f,1.0f,1.0f });
}

void GamePlayScene::ParticleBoxDraw()
{
	explodeParticle_->Draw();
}

void GamePlayScene::PreSpriteDraw()
{

}

void GamePlayScene::PostSpriteDraw()
{
	player_->ReticleDraw();
}

void GamePlayScene::CheckAllCollision()
{
	//敵当たり判定
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		for (const std::unique_ptr<PlayerBullet>& playerBullet : playerBulletManager_->GetPlayerBullets()) {
			bool isHit = enemy->GetCollider().Collision(playerBullet->GetCollider());
			if (isHit) {
				enemy->OnCollision();
				playerBullet->OnCollision();
				explodeParticle_->SetIsEmit(true, MakeTranslation(enemy->GetWorldTransform()->matWorld_));
			}
		}
	}

	//自機当たり判定
	for (const std::unique_ptr<EnemyBullet>& enemyBullet : enemyBulletManager_->GetEnemyBullets()) {
		bool isHit = player_->collider_.Collision(enemyBullet->GetCollider());
		if (isHit) {
			enemyBullet->OnCollision();
			player_->OnCollision();
			explodeParticle_->SetIsEmit(true, MakeTranslation(player_->GetWorldTransform().matWorld_));
		}
	}
}

void GamePlayScene::EnemySpawn(const Vector3& position)
{
	uint32_t modelHandle = ModelManager::Load("sphere");
	// 敵を生成、初期化
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	// 敵キャラに自キャラのアドレスを渡す
	newEnemy->Initialize(modelHandle, position, enemyBulletManager_.get(), player_.get(), { 0.0f,0.0f, -1.0f });
	// 敵を登録する
	enemies_.push_back(std::move(newEnemy));
}

