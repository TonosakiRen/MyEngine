#include "GameScene.h"
#include "externals/imgui/imgui.h"
#include <cassert>
#include "CommandContext.h"
#include "Renderer.h"

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


	directionalLights_.Initialize();
	directionalLights_.Update();


	textureHandle_ = TextureManager::Load("uvChecker.png");

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(textureHandle_, { 0.0f,0.0f });

	player_ = std::make_unique<Player>();
	player_->Initialize("box1x1");

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize("skydome");
	floor_ = std::make_unique<Floor>();
	floor_->Initialize("floor");
	sphere_ = std::make_unique<GameObject>();
	sphere_->Initialize("sphere");
	sphere_->SetPosition({ 0.0f,8.0f,0.0f });
	sphere_->UpdateMatrix();

	dustParticle_ = std::make_unique<DustParticle>();
	dustParticle_->SetIsEmit(true);
	dustParticle_->Initialize(Vector3{ -1.0f,-1.0f,-1.0f }, Vector3{ 1.0f,1.0f,1.0f });

	whiteParticle_ = std::make_unique<WhiteParticle>();
	whiteParticle_->SetIsEmit(true);
	whiteParticle_->Initialize(Vector3{ -1.0f,-1.0f,-1.0f }, Vector3{ 1.0f,1.0f,1.0f });

	compute_ = std::make_unique<Compute>();
	compute_->Initialize();
}

void GameScene::Update(CommandContext& commandContext){
	Collider::SwitchIsDrawCollider();
	//camera light
	{
		// camera
		viewProjection_.DebugMove();
		viewProjection_.Update();
		// light
		ImGui::DragFloat3("light", &directionalLights_.lights_[0].direction.x, 0.01f);
		ImGui::DragFloat4("lightcolor", &directionalLights_.lights_[0].color.x, 0.01f);
		directionalLights_.lights_[0].direction = Normalize(directionalLights_.lights_[0].direction);
		directionalLights_.Update();
	}
	//Scene
	{
		//Scene初期化
		if (sceneRequest_) {
			whiteParticle_->Update();
			scene_ = sceneRequest_.value();
			(this->*SceneInitializeTable[static_cast<size_t>(scene_)])();
			sceneRequest_ = std::nullopt;
		}
		//SceneUpdate
		(this->*SceneUpdateTable[static_cast<size_t>(scene_)])();
	}

	ImGui::DragFloat4("a",&color.x,0.01f,0.0f,1.0f);
	
	auto io = ImGui::GetIO();
	ImGui::Text("%f", io.Framerate);

	compute_->Dispatch(commandContext);
	uint32_t* date = static_cast<uint32_t*>(compute_->GetData());

	int a = date[1];
	ImGui::Text("%d", int(a));

	Quaternion rotation = MakeRotateAxisAngleQuaternion(Normalize(Vector3{1.0f,0.4f,-0.2f}),0.45f);
	Vector3 pointY = { 2.1f,-0.9f,1.3f };
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation);
	Vector3 rotateByQuaternion = RotateVector(pointY, rotation);
	Vector3 rotateByMatrix = Transform(pointY, rotateMatrix);

	
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
		//sphere_->Draw();
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

	Renderer::GetInstance()->ClearMainDepthBuffer();

	//3Dオブジェクト描画
	Model::PreDraw(commandContext, viewProjection_, directionalLights_);
	ModelDraw();
	Model::PostDraw();

	//Particle描画
	Particle::PreDraw(commandContext,viewProjection_);
	ParticleDraw();
	Particle::PostDraw();

	//Particle描画
	ParticleBox::PreDraw(commandContext, viewProjection_, directionalLights_);
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
