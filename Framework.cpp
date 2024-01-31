#include "Framework.h"
#include "Input.h"
#include "Audio.h"
#include "WinApp.h"
bool Framework::endRequest_ = false;

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
	}

	Framework::Finalize();
	Finalize();
}

void Framework::Initialize()
{
	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();

	input = Input::GetInstance();
	input->Initialize(win->GetHInstance(), win->GetHwnd());

	audio = Audio::GetInstance();
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
