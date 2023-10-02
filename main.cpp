#include <Windows.h>
#include "WinApp.h"
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	WinApp* win = nullptr;
	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();
	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}
	}

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();
	return 0;
}