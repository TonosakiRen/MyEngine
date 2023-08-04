#pragma once

#include <array>
#include <Windows.h>
#include <wrl.h>
#include "Mymath.h"

#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include <dinput.h>

/// <summary>
/// 入力
/// </summary>
class Input {
public: // メンバ関数

	struct MouseState {
		DIMOUSESTATE state;
		Vector2 position;
	};

	static Input* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(HINSTANCE hInstance, HWND hwnd);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber);

	/// <summary>
	/// マウス移動量を取得
	/// </summary>
	/// <returns>マウス移動量</returns>
	Vector2 GetMouseMove();

	/// <summary>
	/// ホイールスクロール量を取得する
	/// </summary>
	/// <returns>ホイールスクロール量。奥側に回したら+。Windowsの設定で逆にしてたら逆</returns>
	float GetWheel();

	/// <summary>
	/// マウスの押下をチェック
	/// </summary>
	/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張マウスボタン)</param>
	/// <returns>押されているか</returns>
	bool IsPressMouse(int32_t mouseNumber);

	/// <summary>
	/// 全キー情報取得
	/// </summary>
	/// <param name="keyStateBuf">全キー情報</param>
	const std::array<BYTE, 256>& GetAllKey() { return key_; }

private: // メンバ変数
	Microsoft::WRL::ComPtr<IDirectInput8> dInput_;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> devKeyboard_;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> devMouse_;
	std::array<BYTE, 256> key_{};
	std::array<BYTE, 256> keyPre_{};
	MouseState mouseState_{};
	MouseState preMouseState_{};
private: // シングルトン
	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	const Input& operator=(const Input&) = delete;
};
