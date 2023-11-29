#include "GameScene.h"
#include "externals/imgui/imgui.h"
#include <cassert>

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
	
	viewProjection_.Initialize();
	viewProjection_.translation_.y = 7.0f;
	viewProjection_.translation_.z = -27.0f;


	directionalLight_.Initialize();
	directionalLight_.direction_ = { 1.0f, -1.0f, 1.0f };
	directionalLight_.Update();


	textureHandle_ = TextureManager::Load("uvChecker.png");

	sprite_.reset(Sprite::Create(textureHandle_, { 0.0f,0.0f }));

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize("skydome", &viewProjection_, &directionalLight_);
	floor_ = std::make_unique<Floor>();
	floor_->Initialize("floor", &viewProjection_, &directionalLight_);
	sphere_ = std::make_unique<GameObject>();
	sphere_->Initialize("sphere", &viewProjection_, &directionalLight_);

	player_ = std::make_unique<Player>();
	player_->Initialize("player", &viewProjection_, &directionalLight_);

	dustParticle_ = std::make_unique<DustParticle>();
	dustParticle_->SetIsEmit(true);
	dustParticle_->Initialize(Vector3{ -1.0f,-1.0f,-1.0f }, Vector3{ 1.0f,1.0f,1.0f });

	whiteParticle_ = std::make_unique<WhiteParticle>();
	whiteParticle_->SetIsEmit(true);
	whiteParticle_->Initialize(Vector3{ -1.0f,-1.0f,-1.0f }, Vector3{ 1.0f,1.0f,1.0f });
}

void GameScene::Update(){
	//camera light
	{
		// camera
		viewProjection_.DebugMove();
		viewProjection_.Update();
		// light
		ImGui::DragFloat3("light", &directionalLight_.direction_.x, 0.01f);
		ImGui::DragFloat4("lightcolor", &directionalLight_.color_.x, 0.01f);
		directionalLight_.direction_ = Normalize(directionalLight_.direction_);
		directionalLight_.Update();
	}
	Collider::SwitchIsDrawCollider();
	dustParticle_->Update();
	ImGui::DragFloat4("a",&color.x,0.01f,0.0f,1.0f);
	//Scene初期化
	if (sceneRequest_) {
	whiteParticle_->Update();
		scene_ = sceneRequest_.value();
		(this->*SceneInitializeTable[static_cast<size_t>(scene_)])();
		sceneRequest_ = std::nullopt;
	}
	//SceneUpdate
	(this->*SceneUpdateTable[static_cast<size_t>(scene_)])();
	
	auto io = ImGui::GetIO();
	ImGui::Text("%f", io.Framerate);

	//Vector3 axis = Normalize(Vector3{ 1.0f,1.0f,1.0f });
	//float angle = 0.44f;
	//Matrix4x4 rotateMatrix = MakeRotateAxisAngle(axis, angle);
	//ImGui::Begin("MT3_1_1");
	//ImGui::Text("%f  %f  %f  %f", rotateMatrix.m[0][0], rotateMatrix.m[0][1], rotateMatrix.m[0][2], rotateMatrix.m[0][3]);
	//ImGui::Text("%f  %f  %f  %f", rotateMatrix.m[1][0], rotateMatrix.m[1][1], rotateMatrix.m[1][2], rotateMatrix.m[1][3]);
	//ImGui::Text("%f  %f  %f  %f", rotateMatrix.m[2][0], rotateMatrix.m[2][1], rotateMatrix.m[2][2], rotateMatrix.m[2][3]);
	//ImGui::Text("%f  %f  %f  %f", rotateMatrix.m[3][0], rotateMatrix.m[3][1], rotateMatrix.m[3][2], rotateMatrix.m[3][3]);
	//ImGui::End();

	Vector3 from0 = Normalize(Vector3{ 1.0f,0.7f,0.5f });
	Vector3 to0 = -from0;
	Vector3 from1 = Normalize(Vector3{ -0.6f,0.9f,0.2f });
	Vector3 to1 = Normalize(Vector3{ 0.4f,0.7f,-0.5f });

	Matrix4x4 rotateMatrix0 = DirectionToDirection(Normalize(Vector3{ 1.0f,0.0f,0.0f }),Normalize(Vector3{-1.0f,0.0f,0.0f}));
	Matrix4x4 rotateMatrix1 = DirectionToDirection(from0, to0);
	Matrix4x4 rotateMatrix2 = DirectionToDirection(from1, to1);

	ImGui::Begin("MT3_1_1");
	ImGui::Text("%f  %f  %f  %f", rotateMatrix0.m[0][0], rotateMatrix0.m[0][1], rotateMatrix0.m[0][2], rotateMatrix0.m[0][3]);
	ImGui::Text("%f  %f  %f  %f", rotateMatrix0.m[1][0], rotateMatrix0.m[1][1], rotateMatrix0.m[1][2], rotateMatrix0.m[1][3]);
	ImGui::Text("%f  %f  %f  %f", rotateMatrix0.m[2][0], rotateMatrix0.m[2][1], rotateMatrix0.m[2][2], rotateMatrix0.m[2][3]);
	ImGui::Text("%f  %f  %f  %f", rotateMatrix0.m[3][0], rotateMatrix0.m[3][1], rotateMatrix0.m[3][2], rotateMatrix0.m[3][3]);

	ImGui::Text("");
	ImGui::Text("%f  %f  %f  %f", rotateMatrix1.m[0][0], rotateMatrix1.m[0][1], rotateMatrix1.m[0][2], rotateMatrix1.m[0][3]);
	ImGui::Text("%f  %f  %f  %f", rotateMatrix1.m[1][0], rotateMatrix1.m[1][1], rotateMatrix1.m[1][2], rotateMatrix1.m[1][3]);
	ImGui::Text("%f  %f  %f  %f", rotateMatrix1.m[2][0], rotateMatrix1.m[2][1], rotateMatrix1.m[2][2], rotateMatrix1.m[2][3]);
	ImGui::Text("%f  %f  %f  %f", rotateMatrix1.m[3][0], rotateMatrix1.m[3][1], rotateMatrix1.m[3][2], rotateMatrix1.m[3][3]);

	ImGui::Text("");
	ImGui::Text("%f  %f  %f  %f", rotateMatrix2.m[0][0], rotateMatrix2.m[0][1], rotateMatrix2.m[0][2], rotateMatrix2.m[0][3]);
	ImGui::Text("%f  %f  %f  %f", rotateMatrix2.m[1][0], rotateMatrix2.m[1][1], rotateMatrix2.m[1][2], rotateMatrix2.m[1][3]);
	ImGui::Text("%f  %f  %f  %f", rotateMatrix2.m[2][0], rotateMatrix2.m[2][1], rotateMatrix2.m[2][2], rotateMatrix2.m[2][3]);
	ImGui::Text("%f  %f  %f  %f", rotateMatrix2.m[3][0], rotateMatrix2.m[3][1], rotateMatrix2.m[3][2], rotateMatrix2.m[3][3]);
	ImGui::End();
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
	skydome_->Update();
	floor_->Update();
	player_->Update();
	player_->Collision(floor_->collider_);
}

void GameScene::ModelDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		skydome_->Draw();
		floor_->Draw();
		player_->Draw();
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
		whiteParticle_->Draw(viewProjection_, color);
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
		//dustParticle_->Draw(viewProjection_, directionalLight_);
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
		sprite_->Draw();
		break;
	default:
		break;
	}
}

void GameScene::Draw(CommandContext& commandContext) {

	// 背景スプライト描画
	Sprite::PreDraw(commandContext);
	PreSpriteDraw();
	Sprite::PostDraw();

	DirectXCommon::GetInstance()->ClearMainDepthBuffer();

	//3Dオブジェクト描画
	Model::PreDraw(commandContext);
	ModelDraw();
	Model::PostDraw();

	//Particle描画
	Particle::PreDraw(commandContext);
	ParticleDraw();
	Particle::PostDraw();

	//Particle描画
	ParticleBox::PreDraw(commandContext);
	ParticleBoxDraw();
	ParticleBox::PostDraw();

}

void GameScene::UIDraw(CommandContext& commandContext)
{
	// 前景スプライト描画
	Sprite::PreDraw(commandContext);
	PostSpriteDraw();
	Sprite::PostDraw();
}
