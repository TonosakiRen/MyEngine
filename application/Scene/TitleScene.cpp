/**
 * @file TitleScene.cpp
 * @brief タイトルのシーン
 */
#include "Scene/TitleScene.h"
#include "Texture/TextureManager.h"
#include "GameComponent/GameObject.h"
#include "Texture/Sprite.h"
#include "Graphics/WinApp.h"
#include "Input.h"
#include "Scene/GamePlayScene.h"
#include "Scene/SceneManager.h"
#include "Scene/GameScene.h"
#include "Draw/DrawManager.h"
void TitleScene::Initialize()
{
	input_ = Input::GetInstance();

	title_ = std::make_unique<GameObject>();
	title_->Initialize("title.obj");
	title_->GetWorldTransform()->translation_ = { 0.0f,3.0f,0.0f };
	title_->UpdateMatrix();
	pushSpace_ = std::make_unique<SpriteData>();
	pushSpace_->Initialize(TextureManager::Load("pushSpace.png"), { WinApp::kWindowWidth / 2.0f,WinApp::kWindowHeight / 2.0f + 300.0f });

	BaseScene::currentViewProjection->SetTranslation({0.0f,4.5f,-8.0f});
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
	//SPACE A を押したらGamePlaySceneへ
	if (input_->TriggerKey(DIK_SPACE) || input_->TriggerButton(XINPUT_GAMEPAD_A) || input_->TriggerKey(DIK_A)) {
		BaseScene* scene = new GamePlayScene();
		sceneManager_->SetNextScene(scene);
	}

	// titleを上下させる
	const float moveY = 4.0f;
	const float moveValue = 0.03f;
	if (titleT_ >= 0.5f) {
		isTitleUp_ = false;
	}
	if (titleT_ <= -0.5f) {
		isTitleUp_ = true;
	}

	if (isTitleUp_) {
		titleT_ += moveValue;
	}
	else {
		titleT_ -= moveValue;
	}
	title_->GetWorldTransform()->translation_.y = moveY + titleT_;

	title_->UpdateMatrix();
}

void TitleScene::Draw()
{
	title_->Draw();
	DrawManager::GetInstance()->DrawPostSprite(*pushSpace_);
}