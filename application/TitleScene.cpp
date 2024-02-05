#include "TitleScene.h"
#include "TextureManager.h"
#include "GameObject.h"
#include "Sprite.h"
#include "WinApp.h"
#include "Input.h"
#include "GamePlayScene.h"
#include "SceneManager.h"
#include "GameScene.h"
void TitleScene::Initialize()
{
	input_ = Input::GetInstance();

	title_ = std::make_unique<GameObject>();
	title_->Initialize("title");
	title_->GetWorldTransform()->translation_ = { 0.0f,3.0f,0.0f };
	title_->UpdateMatrix();
	pushSpace_ = std::make_unique<Sprite>();
	pushSpace_->Initialize(TextureManager::Load("pushSpace.png"), { WinApp::kWindowWidth / 2.0f,WinApp::kWindowHeight / 2.0f + 300.0f });

	GameScene::currentViewProjection_->SetTranslation({0.0f,4.5f,-8.0f});
}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
	if (input_->TriggerKey(DIK_SPACE) || input_->TriggerButton(XINPUT_GAMEPAD_A) || input_->TriggerKey(DIK_A)) {
		BaseScene* scene = new GamePlayScene();
		sceneManager_->SetNextScene(scene);
	}
	if (titleT_ >= 0.5f) {
		isTitleUp_ = false;
	}
	if (titleT_ <= -0.5f) {
		isTitleUp_ = true;
	}

	if (isTitleUp_) {
		titleT_ += 0.03f;
	}
	else {
		titleT_ -= 0.03f;
	}
	title_->GetWorldTransform()->translation_.y = 4.0f + titleT_;

	title_->UpdateMatrix();
}

void TitleScene::ModelDraw()
{
	title_->Draw();
}

void TitleScene::ShadowDraw()
{

}

void TitleScene::SpotLightShadowDraw()
{
}

void TitleScene::ParticleDraw()
{
}

void TitleScene::ParticleBoxDraw()
{
}

void TitleScene::PreSpriteDraw()
{
}

void TitleScene::PostSpriteDraw()
{
	pushSpace_->Draw();
}

