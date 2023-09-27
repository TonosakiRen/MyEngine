#include <Windows.h>
#include "WinApp.h"
#include "DirectXCommon.h"
#include "ShaderResourceManager.h"
#include "GameScene.h"
#include "ImGuiManager.h"
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	//汎用機能
	GameScene* gameScene = nullptr;
	Input* input = nullptr;
	ImGuiManager* imguiManager;

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();
	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize();
	// 汎用機能
#pragma region 汎用機能初期化
	input = Input::GetInstance();
	input->Initialize(win->GetHInstance(), win->GetHwnd());

	// テクスチャマネージャの初期化
	ShaderResourceManager::GetInstance()->Initialize();
	ShaderResourceManager::Load("white1x1.png");

	imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win);

	// 3Dオブジェクト静的初期化
	Model::StaticInitialize();
	// 3Dオブジェクト静的初期化
	/*Triangle::StaticInitialize(WinApp::kWindowWidth, WinApp::kWindowHeight);*/

	//　スプライト静的初期化
	Sprite::StaticInitialize(WinApp::kWindowWidth, WinApp::kWindowHeight);

	//　スプライト静的初期化
	Sphere::StaticInitialize(WinApp::kWindowWidth, WinApp::kWindowHeight);

#pragma endregion

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();


	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();

		// 入力関連の毎フレーム処理
		input->Update();

		// ゲームシーンの毎フレーム処理
		gameScene->Update();
		imguiManager->End();


		// ゲームシーンの描画
		gameScene->Draw();
	}


	// ImGui解放
	imguiManager->Finalize();
	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}