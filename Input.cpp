#include "Input.h"
#include <cassert>

#pragma comment(lib, "dinput8.lib")

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {
	HRESULT result = S_FALSE;

	// DirectInputオブジェクトの生成
	result = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dInput_, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイスの生成
	result = dInput_->CreateDevice(GUID_SysKeyboard, &devKeyboard_, NULL);
	assert(SUCCEEDED(result));

	//マウスの生成
	result = dInput_->CreateDevice(GUID_SysMouse, &devMouse_, nullptr);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = devKeyboard_->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	result = devMouse_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = devKeyboard_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	result = devMouse_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update() {
	devKeyboard_->Acquire(); // キーボード動作開始
	devMouse_->Acquire();

	// 前回のキー入力を保存
	keyPre_ = key_;
	preMouseState_ = mouseState_;

	// キーの入力
	devKeyboard_->GetDeviceState((DWORD)size(key_), key_.data());
	devMouse_->GetDeviceState(sizeof(mouseState_.state), &mouseState_.state);
}

bool Input::PushKey(BYTE keyNumber) {

	// 0でなければ押している
	if (key_[keyNumber]) {
		return true;
	}

	// 押していない
	return false;
}

bool Input::TriggerKey(BYTE keyNumber) {

	// 前回が0で、今回が0でなければトリガー
	if (!keyPre_[keyNumber] && key_[keyNumber]) {
		return true;
	}

	// トリガーでない
	return false;
}

Vector2 Input::GetMouseMove() {
	return { static_cast<float>(mouseState_.state.lX) ,static_cast<float>(mouseState_.state.lY) };
}

float Input::GetWheel() {
	return static_cast<float>(mouseState_.state.lZ);
}

bool Input::IsPressMouse(int32_t mouseNumber) {
	return mouseState_.state.rgbButtons[mouseNumber] & 0x80;
}