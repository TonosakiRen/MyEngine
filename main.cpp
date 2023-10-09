#include <Windows.h>
#include "WinApp.h"
#include "renderManager.h"
#include "Engine.h"
#include "Camera.h"
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Engine::Initialize();

	std::unique_ptr<GameObject> box = nullptr;
	box = std::make_unique<GameObject>();
	box->Initialize("box");

	Camera camera ;
	// メインループ
	while (Engine::BeginFrame()) {


		box->Update();
		camera.UpdateMatrices();

		Engine::BeginDraw();
		box->Draw(Engine::renderManager->GetCommandContext(), camera);
		Engine::EndDraw();
	}

	// ゲームウィンドウの破棄
	Engine::Shutdown();
	return 0;
}