#include "GameScene.h"
#include <imgui.h>
#include <cassert>
#include "CommandContext.h"
#include "Renderer.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "ShadowMap.h"

void (GameScene::* GameScene::SceneUpdateTable[])() = {
	&GameScene::TitleUpdate,
	&GameScene::InGameUpdate,
};

void (GameScene::* GameScene::SceneInitializeTable[])() = {
	&GameScene::TitleInitialize,
	&GameScene::InGameInitialize,
};


GameScene::GameScene() {};

GameScene::~GameScene() {};

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	Vector3 initializeCameraPos = { 0.0f,3.2f,-11.0f };
	Vector3 initializeCameraRotate = { 0.06f,0.0f,0.0f };

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(initializeCameraPos, initializeCameraRotate);


	camera_ = std::make_unique<Camera>();
	camera_->Initialize(initializeCameraPos, MakeFromEulerAngle(initializeCameraRotate));

	currentViewProjection_ = debugCamera_.get();

	directionalLights_.Initialize();
	directionalLights_.Update();

	pointLights_.Initialize();
	pointLights_.Update();

	spotLights_.Initialize();
	spotLights_.Update();

	textureHandle_ = TextureManager::Load("uvChecker.png");

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(textureHandle_, { 0.0f,0.0f });

	walls_.resize(3);
	uint32_t wallModelHandle_ = ModelManager::Load("wall");
	float sizeX = ModelManager::GetInstance()->ModelManager::GetModelSize(wallModelHandle_).x;
	walls_[0] = std::make_unique<Wall>();
	walls_[0]->Initialize({ -sizeX ,-1.6f,0.0f});
	walls_[1] = std::make_unique<Wall>();
	walls_[1]->Initialize({ 0.0f,-1.6f,0.0f });
	walls_[2] = std::make_unique<Wall>();
	walls_[2]->Initialize({ +sizeX,-1.6f,0.0f });
	
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

	dustParticle_ = std::make_unique<DustParticle>();
	dustParticle_->SetIsEmit(true);
	dustParticle_->Initialize(Vector3{ -1.0f,-1.0f,-1.0f }, Vector3{ 1.0f,1.0f,1.0f });

	explodeParticle_ = std::make_unique<ExplodeParticle>();
	explodeParticle_->Initialize(Vector3{ -1.0f,-1.0f,-1.0f }, Vector3{ 1.0f,1.0f,1.0f },&pointLights_);

	whiteParticle_ = std::make_unique<WhiteParticle>();
	whiteParticle_->SetIsEmit(true);
	whiteParticle_->Initialize(Vector3{ -1.0f,-1.0f,-1.0f }, Vector3{ 1.0f,1.0f,1.0f });

	compute_ = std::make_unique<Compute>();
	compute_->Initialize();

	enemyBulletManager_ = std::make_unique<EnemyBulletManager>();
	playerBulletManager_ = std::make_unique<PlayerBulletManager>();
	enemyBulletManager_->Initialize();
	playerBulletManager_->Initialize();

	player_ = std::make_unique<Player>();
	player_->Initialize("sphere", playerBulletManager_.get());
}

void GameScene::Update(CommandContext& commandContext){
	Collider::SwitchIsDrawCollider();
	ViewProjection::SwitchIsUseDebugCamera();
	//fps表示
#ifdef _DEBUG
	ImGui::Begin("fps");
	auto io = ImGui::GetIO();
	ImGui::Text("%f", io.Framerate);
	ImGui::End();
#endif
	//camera light
	{
		if (ViewProjection::isUseDebugCamera) {
			currentViewProjection_ = debugCamera_.get();
			debugCamera_->Update();
		}
		else {
			currentViewProjection_ = camera_.get();
			camera_->Update(player_->GetWorldTransform()->translation_);
		}
		
		// light
#ifdef _DEBUG
		ImGui::Begin("DirectionalLight");
		ImGui::DragFloat3("lightDirection", &directionalLights_.lights_[0].direction.x, 0.01f);
		ImGui::DragFloat3("lightPosition", &directionalLights_.lights_[0].position.x, 1.0f);
		ImGui::DragFloat4("lightColor", &directionalLights_.lights_[0].color.x, 1.0f,0.0f,255.0f);
		ImGui::DragFloat("intensity", &directionalLights_.lights_[0].intensity, 0.01f, 0.0f);
		ImGui::End();
#endif
		directionalLights_.lights_[0].direction = Normalize(directionalLights_.lights_[0].direction);
		directionalLights_.Update();

#ifdef _DEBUG
		ImGui::Begin("pointLight");

		ImGui::DragFloat3("lightPosition", &pointLights_.lights_[0].worldTransform.translation_.x, 0.01f);
		ImGui::DragFloat3("lightColor", &pointLights_.lights_[0].color.x, 1.0f, 0.0f, 255.0f);
		ImGui::DragFloat("intensity", &pointLights_.lights_[0].intensity, 0.01f, 0.0f);
		ImGui::DragFloat("radius", &pointLights_.lights_[0].radius, 0.01f, 0.0f);
		ImGui::DragFloat("decay", &pointLights_.lights_[0].decay, 0.01f, 0.0f);
		ImGui::End();

		ImGui::Begin("pointLight2");
		ImGui::DragFloat3("lightPosition", &pointLights_.lights_[1].worldTransform.translation_.x, 0.01f);
		ImGui::DragFloat3("lightColor", &pointLights_.lights_[1].color.x, 1.0f, 0.0f, 255.0f);
		ImGui::DragFloat("intensity", &pointLights_.lights_[1].intensity, 0.01f, 0.0f);
		ImGui::DragFloat("radius", &pointLights_.lights_[1].radius, 0.01f, 0.0f);
		ImGui::DragFloat("decay", &pointLights_.lights_[1].decay, 0.01f, 0.0f);
		ImGui::End();
#endif
		pointLights_.Update();

#ifdef _DEBUG
		ImGui::Begin("spotLight");
		ImGui::DragFloat3("lightPosition", &spotLights_.lights_[0].worldTransform.translation_.x, 0.01f);
		ImGui::DragFloat3("lightColor", &spotLights_.lights_[0].color.x, 1.0f, 0.0f, 255.0f);
		ImGui::DragFloat("intensity", &spotLights_.lights_[0].intensity, 0.01f, 0.0f);
		ImGui::DragFloat3("direction", &spotLights_.lights_[0].direction.x, 0.01f, 0.0f);
		ImGui::DragFloat("distance", &spotLights_.lights_[0].distance, 0.01f, 0.0f);
		ImGui::DragFloat("decay", &spotLights_.lights_[0].decay, 0.01f, 0.0f);
		ImGui::DragFloat("cosAngle", &spotLights_.lights_[0].cosAngle, Radian(1.0f), 0.0f, Radian(179.0f));
		ImGui::End();

		ImGui::Begin("spotLight2");
		ImGui::DragFloat3("lightPosition", &spotLights_.lights_[1].worldTransform.translation_.x, 0.01f);
		ImGui::DragFloat3("lightColor", &spotLights_.lights_[1].color.x, 1.0f, 0.0f, 255.0f);
		ImGui::DragFloat("intensity", &spotLights_.lights_[1].intensity, 0.01f, 0.0f);
		ImGui::DragFloat3("direction", &spotLights_.lights_[1].direction.x, 0.01f, 0.0f);
		ImGui::DragFloat("distance", &spotLights_.lights_[1].distance, 0.01f, 0.0f);
		ImGui::DragFloat("decay", &spotLights_.lights_[1].decay, 0.01f, 0.0f);
		ImGui::DragFloat("cosAngle", &spotLights_.lights_[1].cosAngle, Radian(1.0f), 0.0f, Radian(179.0f));
		ImGui::End();
#endif
		spotLights_.lights_[0].direction = Normalize(spotLights_.lights_[0].direction);
		spotLights_.Update();
	}
	//Scene
	{
		//Scene初期化
		if (sceneRequest_) {
			scene_ = sceneRequest_.value();
			(this->*SceneInitializeTable[static_cast<size_t>(scene_)])();
			sceneRequest_ = std::nullopt;
		}
		//SceneUpdate
		(this->*SceneUpdateTable[static_cast<size_t>(scene_)])();
	}

	//コンピュートシェーダテスト
	{
		compute_->Dispatch(commandContext);
		uint32_t* date = static_cast<uint32_t*>(compute_->GetData());

		int a = date[1];
#ifdef _DEBUG
		ImGui::Text("%d", int(a));
		#endif
	}
	
	//パーティクル
	ImGui::Begin("particle");
	ImGui::DragFloat4("particleColor", &color.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat3("emitterPos", &whiteParticle_->emitterWorldTransform_.translation_.x, 0.01f);
	ImGui::End();
	whiteParticle_->Update();
}

void GameScene::TitleInitialize() {

}
void GameScene::TitleUpdate() {

	if (input_->TriggerKey(DIK_P)) {
		sceneRequest_ = Scene::InGame;
	}

}
void GameScene::InGameInitialize() {

}
void GameScene::InGameUpdate() {

	if (input_->TriggerKey(DIK_P)) {
		sceneRequest_ = Scene::Title;
	}

	boxArea_->Update();

	player_->Update(*currentViewProjection_);

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
		EnemySpawn({ 0.0f,1.0f,0.0f });
	}
	enemySpawnFrame_--;
}

void GameScene::CheckAllCollision()
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
			explodeParticle_->SetIsEmit(true, MakeTranslation(player_->GetWorldTransform()->matWorld_));
		}
	}
}

void GameScene::EnemySpawn(Vector3 position)
{
	uint32_t modelHandle = ModelManager::Load("sphere");
	// 敵を生成、初期化
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	// 敵キャラに自キャラのアドレスを渡す
	newEnemy->Initialize(modelHandle,position, enemyBulletManager_.get(),player_.get(),{0.0f,0.0f, -1.0f});
	// 敵を登録する
	enemies_.push_back(std::move(newEnemy));
}

void GameScene::ModelDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		boxArea_->Draw();
		player_->Draw();
		sphere_->Draw();
		//敵描画
		for (const std::unique_ptr<Enemy>& enemy : enemies_) {
			enemy->Draw();
		}

		enemyBulletManager_->Draw();
		playerBulletManager_->Draw();
		break;
	default:
		break;
	}
	
}

void GameScene::ShadowDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		player_->Draw();
		sphere_->Draw();
		break;
	default:
		break;
	}
}

void GameScene::ParticleDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		whiteParticle_->Draw(color);
		break;
	default:
		break;
	}
}

void GameScene::ParticleBoxDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		dustParticle_->Draw();
		explodeParticle_->Draw();
		break;
	default:
		break;
	}
}

void GameScene::PreSpriteDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		break;
	default:
		break;
	}
}

void GameScene::PostSpriteDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		//sprite_->Draw();
		player_->ReticleDraw();
		break;
	default:
		break;
	}
}

void GameScene::Draw(CommandContext& commandContext) {

	// 背景スプライト描画
	Sprite::PreDraw(&commandContext);
	PreSpriteDraw();
	Sprite::PostDraw();

	Renderer::GetInstance()->ClearMainDepthBuffer();

	//3Dオブジェクト描画
	Model::PreDraw(&commandContext, *currentViewProjection_ , directionalLights_);
	ModelDraw();
	Model::PostDraw();

	//Particle描画
	Particle::PreDraw(&commandContext, *currentViewProjection_);
	ParticleDraw();
	Particle::PostDraw();

	//Particle描画
	ParticleBox::PreDraw(&commandContext, *currentViewProjection_);
	ParticleBoxDraw();
	ParticleBox::PostDraw();

}

void GameScene::ShadowMapDraw(CommandContext& commandContext)
{
	ShadowMap::PreDraw(&commandContext, directionalLights_);
	ShadowDraw();
	ShadowMap::PostDraw();
}

void GameScene::UIDraw(CommandContext& commandContext)
{
	// 前景スプライト描画
	Sprite::PreDraw(&commandContext);
	PostSpriteDraw();
	Sprite::PostDraw();
}
