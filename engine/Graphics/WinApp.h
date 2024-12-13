#pragma once
/**
 * @file WinApp.h
 * @brief WindowsAPIラップ
 */
#include <Windows.h>
#include <cstdint>

class WinApp
{
public:
	static const int kWindowWidth = 1280; 
	static const int kWindowHeight = 720; 
	static const wchar_t kWindowClassName[];

	RECT windowRect_;
	HWND hwnd_ = nullptr;
	WNDCLASS wndClass_{};

public:
	static WinApp* GetInstance();
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	void CreateGameWindow(const char* title = "DirextXGame");
	//Messageを受け付け
	bool ProcessMessage();
	// ウィンドウクラスを登録解除
	void TerminateGameWindow();
	
	HWND GetHwnd() const { return hwnd_; }
	HINSTANCE GetHInstance() const { return wndClass_.hInstance; }
private: 
	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp&) = delete;
	const WinApp& operator=(const WinApp&) = delete;
};

