/**
 * @file MyGame.h
 * @brief Gameの更新描画を行う
 */
#include "Framework/MyGame.h"

#include <Windows.h>
#include "Light/ShaderManager.h"
#include "Render/Renderer.h"
#include "GameComponent/GameObjectManager.h"
#include "Light/LightManager.h"
#include "GPUResource/BufferManager.h"
#include "Scene/GamePlayScene.h"
#include "Scene/SceneManager.h"
#include "audio/Audio.h"
#include "ImGui/ImGuiManager.h"


void MyGame::Initialize()
{

	renderer = Renderer::GetInstance();
	renderer->Initialize();

	bufferManager = BufferManager::GetInstance();

	// テクスチャマネージャの初期化
	textureManager = Engine::TextureManager::GetInstance();
	textureManager->Initialize(renderer->GetCommandContext());
	textureManager->Load("white1x1.png");

	modelManager = Engine::ModelManager::GetInstance();
	modelManager->Initialize(renderer->GetCommandContext());
	modelManager->Load("box1x1.obj");

	lightManager_ = LightManager::GetInstance();
	lightManager_->Initialize();

	sceneManager = SceneManager::GetInstance();
	BaseScene::StaticInitialize();
	BaseScene* scene = new GamePlayScene();
	sceneManager->SetNextScene(scene);

	gameObjectManager = GameObjectManager::GetInstance();
}

void MyGame::Finalize()
{

	sceneManager->Finalize();
	modelManager->Finalize();
	textureManager->Finalize();
	lightManager_->Finalize();
	gameObjectManager->Finalize();
	renderer->Finalize();
	bufferManager->Finalize();
}

void MyGame::Update()
{

	renderer->BeginFrame();

	// 入力関連の毎フレーム処理
	input->Update();
	audio->Update();

	// ゲームシーンの毎フレーム処理
	sceneManager->Update();
	if (sceneManager->GetNextScene()) {
		renderer->StartTransition();
	};

	//Light
	{
#ifdef USE_IMGUI
		ImGui::Begin("Game");
		if (ImGui::BeginMenu("DirectionalLight")) {
			ImGui::DragFloat3("lightDirection", &lightManager_->directionalLights_->lights_[0].direction.x, 0.01f);
			ImGui::DragFloat3("lightPosition", &lightManager_->directionalLights_->lights_[0].position.x, 1.0f);
			ImGui::DragFloat4("lightColor", &lightManager_->directionalLights_->lights_[0].color.x, 1.0f, 0.0f, 255.0f);
			ImGui::DragFloat("intensity", &lightManager_->directionalLights_->lights_[0].intensity, 0.01f, 0.0f);
			ImGui::EndMenu();
		}
#endif
		lightManager_->directionalLights_->lights_[0].direction = Normalize(lightManager_->directionalLights_->lights_[0].direction);

#ifdef USE_IMGUI
		if (ImGui::BeginMenu("areaLights")) {
			ImGui::DragFloat3("origin", &lightManager_->areaLights_->lights_[0].segment.origin.x);
			ImGui::DragFloat3("diff", &lightManager_->areaLights_->lights_[0].segment.diff.x);
			ImGui::DragFloat3("lightColor", &lightManager_->areaLights_->lights_[0].color.x, 1.0f, 0.0f, 255.0f);
			ImGui::DragFloat("intensity", &lightManager_->areaLights_->lights_[0].intensity, 0.01f, 0.0f);
			ImGui::DragFloat("range", &lightManager_->areaLights_->lights_[0].range, 0.01f, 0.0f);
			ImGui::DragFloat("decay", &lightManager_->areaLights_->lights_[0].decay, 0.01f, 0.0f);
			ImGui::EndMenu();
		};
		ImGui::End();

#endif

	}
	lightManager_->Update();

}

void MyGame::Draw()
{
	sceneManager->Draw();

	renderer->Render(*BaseScene::currentViewProjection);
}
