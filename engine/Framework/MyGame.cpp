#include "MyGame.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include <Windows.h>
#include "GameScene.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "GameObjectManager.h"
#include "LightManager.h"
#include "BufferManager.h"


void MyGame::Initialize()
{

	renderer = Renderer::GetInstance();
	renderer->Initialize();

	bufferManager = BufferManager::GetInstance();

	// テクスチャマネージャの初期化
	textureManager = TextureManager::GetInstance();
	textureManager->Initialize(renderer->GetCommandContext());
	textureManager->Load("white1x1.png");

	modelManager = ModelManager::GetInstance();
	modelManager->Initialize(renderer->GetCommandContext());
	modelManager->Load("box1x1.obj");

	lightManager_ = LightManager::GetInstance();
	lightManager_->Initialize();

	sceneManager = SceneManager::GetInstance();

	gameObjectManager = GameObjectManager::GetInstance();
#pragma endregion 変数

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();
}

void MyGame::Finalize()
{

	sceneManager->Finalize();
	modelManager->Finalize();
	textureManager->Finalize();
	lightManager_->Finalize();
	gameObjectManager->Finalize();
	renderer->Shutdown();
	bufferManager->Finalize();

	delete gameScene;


}

void MyGame::Update()
{

	renderer->BeginFrame();

	// 入力関連の毎フレーム処理
	input->Update();
	audio->Update();

	// ゲームシーンの毎フレーム処理
	gameScene->Update(renderer->GetCommandContext());
	lightManager_->Update();
	if (sceneManager->GetNextScene()) {
		renderer->StartTransition();
	};

}

void MyGame::Draw()
{
	// 描画開始

	gameScene->Draw();

	renderer->ShadowMapRender();

	renderer->SpotLightShadowMapRender();

	renderer->MainRender(gameScene->GetViewProjection());

	renderer->DeferredRender(gameScene->GetViewProjection());

	renderer->UIRender();

	renderer->EndRender();

	
}