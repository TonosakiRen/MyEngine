#pragma once
#include <Windows.h>
#include <cstdint>

class WinApp
{
public:
	static WinApp* GetInstance();
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	void Initialize(const char* title, uint32_t clientWidth, uint32_t clientHeight);
	bool ProcessMessage();
	void Shutdown();
	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns></returns>
	HWND GetHwnd() const { return hwnd_; }
	HINSTANCE GetHInstance() const { return wndClass_.hInstance; }
public:
	// ウィンドウサイズ
	uint32_t clientWidth = 1280; // 横幅
	uint32_t clientHeight = 720; // 縦幅
	// ウィンドウクラス名
	static const wchar_t kWindowClassName[];

	//Windous関係
	RECT windowRect_;
	HWND hwnd_ = nullptr;
	WNDCLASS wndClass_{};

private:
	// シングルトン
	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp&) = delete;
	const WinApp& operator=(const WinApp&) = delete;
};

