/**
 * @file Framework.h
 * @brief Frameworkを行う
 */
#include "Framework/Framework.h"

#include "Graphics/WinApp.h"
#include "Scene/SceneManager.h"
bool Framework::endRequest_ = false;
uint32_t Framework::kFrame = 0;
uint32_t Framework::kFrameRemainder = 0;
void Framework::Run()
{
	Framework::Initialize();
	Initialize();

	// メインループ
	while (true) {
		Framework::Update();
		Update();

		if (endRequest_) {
			break;
		}

		Draw();
		kFrame++;
		kFrameRemainder = kFrame % 2;
	}

	Framework::Finalize();
	Finalize();
}

void Framework::Initialize()
{
	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();

	input = Engine::Input::GetInstance();
	input->Initialize(win->GetHInstance(), win->GetHwnd());

	audio = Engine::Audio::GetInstance();
	audio->Initialize();
}

void Framework::Finalize()
{

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();
}

void Framework::Update()
{

	// メッセージ処理
	if (win->ProcessMessage()) {
		endRequest_ = true;
	}
}

void Framework::Draw()
{
}
