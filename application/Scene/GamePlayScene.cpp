/**
 * @file GamePlayScene.cpp
 * @brief gamePlayのSceneクラス
 */
#include "Scene/GamePlayScene.h"
#include "Scene/GameScene.h"

#include "Model/ModelManager.h"
#include "ImGuiManager.h"
#include "GameComponent/GameObjectManager.h"

const Player* GamePlayScene::player = nullptr;

void GamePlayScene::Initialize()
{

	floor_ = std::make_unique<Floor>();
	floor_->Initialize("floor.obj",GameScene::wavePoints);

	sphere_ = std::make_unique<GameObject>();
	sphere_->Initialize("sphere.obj");
	sphere_->SetPosition({ 0.0f,0.0f,0.0f });
	sphere_->SetScale({ 1.0f,1.0f,1.0f });
	sphere_->UpdateMatrix();

	skybox_ = std::make_unique<Skybox>();
	skybox_->Initialize("box1x1Inverse.obj");

	explodeParticle_ = std::make_unique<ExplodeParticle>();
	explodeParticle_->Initialize(floor_.get(), GameScene::wavePoints);

	playerBulletManager_ = std::make_unique<PlayerBulletManager>();
	playerBulletManager_->Initialize(explodeParticle_.get());

	player_ = std::make_unique<Player>();
	player_->Initialize("walk.gltf", playerBulletManager_.get());
	player = player_.get();


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

	rainManager_ = std::make_unique<RainManager>();
	rainManager_->Initialize(explodeParticle_.get());

	cave_ = std::make_unique<Cave>();
	cave_->Initialize();
}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::Update()
{

	skybox_->Update();

	trees_->Update();

	player_->Update(*GameScene::currentViewProjection);


	floor_->Update();

	if (!lineAttack_->GetIsEmit()) {
		//lineAttack_->Emit();
	}

	lineAttack_->Update();


	//当たり判定
	CheckAllCollision();

	//敵弾更新
	playerBulletManager_->Update();

	explodeParticle_->Update();

	cave_->Update();

	rainManager_->Update();
	
	for (auto& rainDrop : rainManager_->GetRainDrop()) {
		if (rainDrop.IsActive()) {
			if (rainDrop.GetCollider().Collision(player_->collider_)) {
				rainDrop.OnCollision();
				player_->SetColor(rainDrop.GetColor());
			}
		}
	}

	cave_->GetMushrooms()->ChangeColorSphere(player_->GetSphereCollider(), player_->GetColor());
}

void GamePlayScene::Draw()
{

	cave_->Draw();

	player_->Draw();


	lineAttack_->Draw();
	explodeParticle_->Draw();
	trees_->Draw();
	

	playerBulletManager_->Draw();

	skybox_->Draw();

	floor_->Draw();

	rainManager_->Draw();
}

void GamePlayScene::CheckAllCollision()
{
	
}

