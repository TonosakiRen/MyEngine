#include "MyGame.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include <Windows.h>
#include "GameScene.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "GameObjectManager.h"


void MyGame::Initialize()
{

	renderer = Renderer::GetInstance();
	renderer->Initialize();

	// テクスチャマネージャの初期化
	textureManager = TextureManager::GetInstance();
	textureManager->Initialize(renderer->GetCommandContext());
	textureManager->Load("white1x1.png");

	modelManager = ModelManager::GetInstance();

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
	gameObjectManager->Finalize();

	delete gameScene;

	// ImGui解放
	renderer->Shutdown();


}

void MyGame::Update()
{

	renderer->BeginFrame();

	// 入力関連の毎フレーム処理
	input->Update();
	audio->Update();

	// ゲームシーンの毎フレーム処理
	gameScene->Update(renderer->GetCommandContext());
	
	if (sceneManager->GetNextScene()) {
		renderer->StartTransition();
	};

}

void MyGame::Draw()
{
	// 描画開始

	gameScene->Draw();

	renderer->ShadowMapRender(gameScene->GetDirectionalLights());

	renderer->SpotLightShadowMapRender(gameScene->GetShadowSpotLights());

	renderer->MainRender(gameScene->GetViewProjection());

	renderer->DeferredRender(gameScene->GetViewProjection(), gameScene->GetDirectionalLights(), gameScene->GetPointLights(), gameScene->GetAreaLights(), gameScene->GetSpotLights(), gameScene->GetShadowSpotLights());

	renderer->UIRender();

	renderer->EndRender();

	
}
