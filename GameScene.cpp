#include "GameScene.h"
#include "externals/imgui/imgui.h"
#include <cassert>
#include "Easing.h"

using namespace DirectX;

GameScene::GameScene() {};

GameScene::~GameScene() {};

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	viewProjection_.Initialize();

	viewProjection_.translation_ = { 0.0f,8.6f,-27.0f };
	viewProjection_.target_ = { 0.0f,0.0f,0.0f };

	followCamera_.Initialize();

	directionalLight_.Initialize();
	directionalLight_.direction_ = { 0.5f, -1.0f, 0.5f };
	directionalLight_.UpdateDirectionalLight();


	textureHandle_ = TextureManager::Load("uvChecker.png");

	sprite_.reset(Sprite::Create(textureHandle_, { 0.0f,0.0f }));

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize("skydome", &followCamera_.GetViewProjection(), &directionalLight_);
	floor_ = std::make_unique<Floor>();
	floor_->Initialize("floor", &followCamera_.GetViewProjection(), &directionalLight_);
	player_ = std::make_unique<Player>();
	player_->Initialize("player", &followCamera_.GetViewProjection(), &directionalLight_);

	followCamera_.SetTarget(player_->GetWorldTransform());

	bosses_.push_back(std::make_unique<Boss>(&followCamera_.GetViewProjection(), &directionalLight_, Vector3{ -20.0f,10.0f,0.0f }));
	

	sphere_.reset(GameObject::Create("sphere", &followCamera_.GetViewProjection(), &directionalLight_));

	particle_.reset(Particle::Create(10));

	ground_ = std::make_unique<Ground>();
	ground_->Initialize(&followCamera_.GetViewProjection(), &directionalLight_, { 0.0f,-5.0f,0.0f });

	bossGround_ = std::make_unique<Ground>();
	bossGround_->Initialize(&followCamera_.GetViewProjection(), &directionalLight_, { -18.0f,-5.0f,0.0f });

	goalGround_ = std::make_unique<Ground>();
	goalGround_->Initialize(&followCamera_.GetViewProjection(), &directionalLight_, { 0.0f,-5.0f,30.0f });
	goalGround_->isMove_ = true;
	blockHandle_ = TextureManager::Load("block.png");

	goalBox_ = std::make_unique<GoalBox>();
	goalBox_->Initialize(&followCamera_.GetViewProjection(), &directionalLight_, { 0.0f,2.0f,30.0f });
}

void GameScene::Update() {
	//camera light
	{
		// camera
		viewProjection_.DebugMove();
		viewProjection_.UpdateMatrix();

		followCamera_.Update();

		directionalLight_.direction_ = Normalize(directionalLight_.direction_);
		directionalLight_.UpdateDirectionalLight();
	}
	Collider::SwitchIsDrawCollider();

	skydome_->Update();
	floor_->Update();
	player_->Update();


	for (const auto& boss : bosses_) {
		boss->Update();
	}

	ground_->Update();

	bossGround_->Update();
	goalGround_->Update();

	for (const auto& boss : bosses_) {
		if (player_->collider.Collision(boss->collider_) || player_->collider.Collision(goalBox_->collider_)) {
			if (boss->GetIsDead() == false) {
				player_->SetInitialPos();
			}
		}
	}

	for (const auto& boss : bosses_) {
		if (player_->weaponCollision(boss->collider_)) {
			boss->OnCollision();
		};
	}

	player_->Collision(ground_->collider_);
	player_->Collision(bossGround_->collider_);
	player_->Collision(goalGround_->collider_);
}

void GameScene::ModelDraw()
{
	skydome_->Draw();
	player_->Draw();
	for (const auto& boss : bosses_) {
		boss->Draw();
	}
	ground_->Draw();
	bossGround_->Draw();
	goalGround_->Draw();
	goalBox_->Draw();
}

void GameScene::ParticleDraw()
{


}

void GameScene::ParticleBoxDraw()
{
	player_->ParticleDraw();
}

void GameScene::PreSpriteDraw()
{

}

void GameScene::PostSpriteDraw()
{

}



void GameScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 背景スプライト描画
	Sprite::PreDraw(commandList);
	PreSpriteDraw();
	Sprite::PostDraw();

	//深度バッファクリア
	dxCommon_->ClearDepthBuffer();

	//3Dオブジェクト描画
	Model::PreDraw(commandList);
	ModelDraw();
	Model::PostDraw();

	//Particle描画
	Particle::PreDraw(commandList);
	ParticleDraw();
	Particle::PostDraw();

	//ParticleBox描画
	ParticleBox::PreDraw(commandList);
	ParticleBoxDraw();
	ParticleBox::PostDraw();

	// 前景スプライト描画
	Sprite::PreDraw(commandList);
	PostSpriteDraw();
	Sprite::PostDraw();
}