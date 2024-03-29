#include "MyGame.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include <Windows.h>
#include "GameScene.h"
#include "Particle.h"
#include "ParticleBox.h"
#include "PostEffect.h"
#include "GaussianBlur.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "Compute.h"
#include "ShadowMap.h"
#include "SpotLightShadowMap.h"
#include "Model.h"
#include "Sprite.h"


void MyGame::Initialize()
{

	renderer = Renderer::GetInstance();
	renderer->Initialize();

	// テクスチャマネージャの初期化
	textureManager = TextureManager::GetInstance();
	textureManager->Initialize();
	textureManager->Load("white1x1.png");

	modelManager = ModelManager::GetInstance();


	// 3Dオブジェクト静的初期化
	Model::StaticInitialize();
	ShadowMap::StaticInitialize();
	SpotLightShadowMap::StaticInitialize();
	Particle::StaticInitialize();
	ParticleBox::StaticInitialize();
	Sprite::StaticInitialize();

#pragma endregion 変数

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();
}

void MyGame::Finalize()
{

	modelManager->Finalize();
	textureManager->Finalize();
	Model::Finalize();
	ShadowMap::Finalize();
	SpotLightShadowMap::Finalize();
	Particle::Finalize();
	ParticleBox::Finalize();
	Sprite::Finalize();

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
	
	if (gameScene->GetIsSceneChange()) {
		renderer->StartTransition();
		gameScene->SetIsSceneChange(!renderer->GetIsNextScene());
	};

}

void MyGame::Draw()
{
	// 描画開始
	renderer->BeginShadowMapRender(gameScene->GetDirectionalLights());

	gameScene->ShadowMapDraw(renderer->GetCommandContext());

	renderer->EndShadowMapRender(gameScene->GetDirectionalLights());

	renderer->BeginSpotLightShadowMapRender(gameScene->GetShadowSpotLights());

	gameScene->SpotLightShadowMapDraw(renderer->GetCommandContext());

	renderer->EndSpotLightShadowMapRender(gameScene->GetShadowSpotLights());

	renderer->BeginMainRender();

	// ゲームシーンの描画
	gameScene->Draw(renderer->GetCommandContext());

	renderer->DeferredRender(gameScene->GetViewProjection(), gameScene->GetDirectionalLights(), gameScene->GetPointLights(), gameScene->GetSpotLights(), gameScene->GetShadowSpotLights());

	renderer->EndMainRender();

	renderer->BeginUIRender();

	gameScene->UIDraw(renderer->GetCommandContext());

	renderer->EndUIRender();
}
