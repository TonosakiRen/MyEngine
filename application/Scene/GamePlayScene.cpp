#include "Scene/GamePlayScene.h"
#include "Scene/GameScene.h"

#include "Model/ModelManager.h"
#include "ImGuiManager.h"
#include "GameComponent/GameObjectManager.h"

const Player* GamePlayScene::player = nullptr;

void GamePlayScene::Initialize()
{
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize("skydome.obj");
	skydome_->Update();

	floor_ = std::make_unique<Floor>();
	floor_->Initialize("floor.obj",GameScene::wavePoints);

	sphere_ = std::make_unique<GameObject>();
	sphere_->Initialize("sphere.obj");
	sphere_->SetPosition({ 0.0f,0.0f,0.0f });
	sphere_->SetScale({ 1.0f,1.0f,1.0f });
	sphere_->UpdateMatrix();

	boxArea_ = std::make_unique<BoxArea>();
	boxArea_->Initialize("box1x1Inverse.obj");

	skybox_ = std::make_unique<Skybox>();
	skybox_->Initialize("box1x1Inverse.obj");

	explodeParticle_ = std::make_unique<ExplodeParticle>();
	explodeParticle_->Initialize(floor_.get(), GameScene::wavePoints);

	whiteParticle_ = std::make_unique<WhiteParticle>();
	whiteParticle_->SetIsEmit(true);
	whiteParticle_->Initialize(Vector3{ -1.0f,-1.0f,-1.0f }, Vector3{ 1.0f,1.0f,1.0f });

	enemyBulletManager_ = std::make_unique<EnemyBulletManager>();
	playerBulletManager_ = std::make_unique<PlayerBulletManager>();
	enemyBulletManager_->Initialize(explodeParticle_.get());
	playerBulletManager_->Initialize(explodeParticle_.get());

	player_ = std::make_unique<Player>();
	player_->Initialize("walk.gltf", playerBulletManager_.get());
	player = player_.get();

	boss_ = std::make_unique<Boss>();
	boss_->Initialize("box1x1.obj");
	boss_->SetPlayer(*player_.get());

	sphereLights_ = std::make_unique<SphereLights>();
	sphereLights_->Initialize();

	GameObjectManager::GetInstance()->Load();
	gameObjects_ = &GameObjectManager::GetInstance()->gameObjects_;

	for (auto& gameObject : *gameObjects_) {
		gameObject->Initialize();
	}

	trees_ = std::make_unique<Trees>();
	trees_->Initialize();

	lineAttack_ = std::make_unique<LineAttack>();
	lineAttack_->Initialize();

	input_ = Input::GetInstance();
}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::Update()
{
	boxArea_->Update();

	skybox_->Update();

	trees_->Update();

	player_->Update(*GameScene::currentViewProjection);

	boss_->Update();

	floor_->Update();

	if (!lineAttack_->GetIsEmit()) {
		lineAttack_->Emit();
	}

	lineAttack_->Update();

	//敵更新
	
	for (const auto& enemy : enemies_) {
		enemy->Update();
	}

	// デスフラグの立った敵を削除
	enemies_.remove_if([](std::unique_ptr<Enemy>& enemy) {
		if (enemy->IsDead()) {
			return true;
		}
		return false;
	});

	//当たり判定
	CheckAllCollision();

	

	//敵弾更新
	enemyBulletManager_->Update();

	//敵弾更新
	playerBulletManager_->Update();

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
	ImGui::Begin("Game");
	if (ImGui::BeginMenu("Particle")) {
		ImGui::DragFloat3("emitterPos", &whiteParticle_->emitterWorldTransform_.translation_.x, 0.01f);
		ImGui::EndMenu();
	}
	ImGui::End();
#endif
	whiteParticle_->Update();

	sphereLights_->Update();
}

void GamePlayScene::Draw()
{
	player_->Draw();

	boss_->Draw();

	lineAttack_->Draw();
	//sphere_->Draw();
	explodeParticle_->Draw();
	trees_->Draw();
	
	//敵描画
	
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Draw();
	}
	

	enemyBulletManager_->Draw();
	playerBulletManager_->Draw();

	skybox_->Draw();
	//whiteParticle_->Draw();
	sphereLights_->Draw();
	floor_->Draw();

	for (auto& gameObject : *gameObjects_) {
		//gameObject->Draw();
	}
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

	//boss当たり判定
	for (const std::unique_ptr<PlayerBullet>& playerBullet : playerBulletManager_->GetPlayerBullets()) {
		bool isHit = boss_->collider_.Collision(playerBullet->GetCollider());
		if (isHit) {
			playerBullet->OnCollision();
			explodeParticle_->SetIsEmit(true, MakeTranslation(playerBullet->GetWorldTransform()->matWorld_));
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
	uint32_t modelHandle = ModelManager::Load("sphere.obj");
	// 敵を生成、初期化
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	// 敵キャラに自キャラのアドレスを渡す
	float direction = 1.0f;
	if (Rand()) {
		direction *= -1.0f;
	}
	newEnemy->Initialize(modelHandle, position, enemyBulletManager_.get(), player_.get(), { direction,0.0f, 0.0f });
	// 敵を登録する
	enemies_.push_back(std::move(newEnemy));
}


