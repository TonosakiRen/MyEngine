/**
 * @file main.cpp
 * @brief main関数
 */
#include "Framework/MyGame.h"
#include "Framework/Framework.h"

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgidebug.h>
#include <dxgi1_3.h>

#pragma comment(lib,"dxguid.lib")

struct ResourceLeakChecker {
	~ResourceLeakChecker() {
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		}
	}
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	ResourceLeakChecker leakCheck;
	Framework* game = new MyGame;

	game->Run();
	
	delete game;

	return 0;
}