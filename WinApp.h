#pragma once
#include <Windows.h>
#include <cstdint>
/// <summary>
/// ウィンドウズアプリケーション
/// </summary>
class WinApp
{
public:
	// ウィンドウサイズ
	static const int kWindowWidth = 1280; // 横幅
	static const int kWindowHeight = 720; // 縦幅
	// ウィンドウクラス名
	static const wchar_t kWindowClassName[];

	//Windous関係
	RECT windowRect_;
	HWND hwnd_ = nullptr;
	WNDCLASS wndClass_{};

public:
	static WinApp* GetInstance();
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	void CreateGameWindow(const char* title = "DirextXGame");
	bool ProcessMessage();
	void TerminateGameWindow();
	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns></returns>
	HWND GetHwnd() const { return hwnd_; }
	HINSTANCE GetHInstance() const { return wndClass_.hInstance; }
private: // シングルトン
	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp&) = delete;
	const WinApp& operator=(const WinApp&) = delete;
};

