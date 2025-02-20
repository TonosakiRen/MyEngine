#pragma once
/**
 * @file Input.h
 * @brief コントローラーなど受付
 */
#include <array>
#include <Windows.h>
#include <wrl.h>
#include "Mymath.h"
#include <cmath>

#define DIRECTINPUT_VERSION 0x0800 
#include <dinput.h>
#include <Xinput.h>

namespace Engine {


	class Input {
	public:

		struct MouseState {
			DIMOUSESTATE state;
			Vector2 position;
		};

		static Input* GetInstance();

		void Initialize(HINSTANCE hInstance, HWND hwnd);
		void Update();

		//Keyが押されているか
		bool PushKey(BYTE keyNumber);
		//Keyがトリガーされているか
		bool TriggerKey(BYTE keyNumber);
		//Keyがリリースされているか
		bool ReleaseKey(BYTE keyNumber);
		//Mouseがどこにあるか
		Vector2 GetMousePosition();
		//Mouseのホイールの回る量
		float GetWheel();
		//マウスのボタン画押されているか(0が左1が右)
		bool IsPressMouse(int32_t mouseNumber);
		const std::array<BYTE, 256>& GetAllKey() { return key_; }

		//右のトリガーがトリガーされているか
		bool TriggerRightTrigger() {
			if (xInputState_.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD && preXInputState_.Gamepad.bRightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
				return true;
			}
			return false;
		}

		//左のトリガーがトリガーされているか
		bool TriggerLeftTrigger() {
			if (xInputState_.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD && preXInputState_.Gamepad.bLeftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
				return true;
			}
			return false;
		}

		//右のトリガーが押されているか
		bool PushRightTrigger() {
			if (xInputState_.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
				return true;
			}
			return false;
		}

		//左のトリガーが押されているか
		bool PushLeftTrigger() {
			if (xInputState_.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
				return true;
			}
			return false;
		}

		//ボタンが押されているか
		bool PushButton(int gamePadButton) {
			if (xInputState_.Gamepad.wButtons & gamePadButton) {
				return true;
			}
			return false;
		}

		//ボタンがトリガーされているか
		bool TriggerButton(int gamePadButton) {
			if (xInputState_.Gamepad.wButtons & gamePadButton &&
				!(preXInputState_.Gamepad.wButtons & gamePadButton)) {
				return true;
			}
			return false;
		}

		//ボタンがリリースされているか
		bool ReleaseButton(int gamePadButton) {
			if (!(xInputState_.Gamepad.wButtons & gamePadButton) &&
				preXInputState_.Gamepad.wButtons & gamePadButton) {
				return true;
			}
			return false;
		}

		//ゲームパッド構造体
		XINPUT_GAMEPAD GetGamePad() {
			return xInputState_.Gamepad;
		}

		//LStickを取得
		Vector2 GetLStick() {
			Vector2 l;
			if (std::fabs(xInputState_.Gamepad.sThumbLX) <= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
				l.x = 0.0f;
			}
			else {
				l.x = static_cast<float>(xInputState_.Gamepad.sThumbLX);
			}
			if (std::fabs(xInputState_.Gamepad.sThumbLY) <= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
				l.y = 0.0f;
			}
			else {
				l.y = static_cast<float>(xInputState_.Gamepad.sThumbLY);
			}
			return l;
		}

		//RStickを取得
		Vector2 GetRStick() {
			Vector2 r;
			if (std::fabs(xInputState_.Gamepad.sThumbRX) <= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
				r.x = 0.0f;
			}
			else {
				r.x = static_cast<float>(xInputState_.Gamepad.sThumbRX);
			}
			if (std::fabs(xInputState_.Gamepad.sThumbRY) <= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
				r.y = 0.0f;
			}
			else {
				r.y = static_cast<float>(xInputState_.Gamepad.sThumbRY);
			}
			return r;
		}

		//LSticKが下に傾いているか
		bool DownLStick(SHORT deadZone = -20000) {
			if (xInputState_.Gamepad.sThumbLY < deadZone) {
				return true;
			}
			return false;
		}
		//LSticKが上に傾いているか
		bool UpLStick(SHORT deadZone = 20000) {
			if (xInputState_.Gamepad.sThumbLY < deadZone) {
				return true;
			}
			return false;
		}
		//LSticKが左に傾いているか
		bool DownLeftLStick(SHORT deadZone = -20000) {
			if (xInputState_.Gamepad.sThumbLX < deadZone) {
				return true;
			}
			return false;
		}
		//LSticK右に傾いているか
		bool DownRightLStick(SHORT deadZone = 20000) {
			if (xInputState_.Gamepad.sThumbLX < deadZone) {
				return true;
			}
			return false;
		}
		//RSticK下に傾いているか
		bool DownRStick(SHORT deadZone = -20000) {
			if (xInputState_.Gamepad.sThumbRY < deadZone) {
				return true;
			}
			return false;
		}
		//RSticK上に傾いているか
		bool UpRStick(SHORT deadZone = 20000) {
			if (xInputState_.Gamepad.sThumbRY < deadZone) {
				return true;
			}
			return false;
		}
		//RSticK左に傾いているか
		bool DownLeftRStick(SHORT deadZone = -20000) {
			if (xInputState_.Gamepad.sThumbRX < deadZone) {
				return true;
			}
			return false;
		}
		//RSticK右に傾いているか
		bool DownRightRStick(SHORT deadZone = 20000) {
			if (xInputState_.Gamepad.sThumbRX < deadZone) {
				return true;
			}
			return false;
		}
		//ゲームパッドが接続されているか
		bool GetIsGamePadConnect() {
			return isGamePadConnect;
		}

	private:
		Microsoft::WRL::ComPtr<IDirectInput8> dInput_;
		Microsoft::WRL::ComPtr<IDirectInputDevice8> devKeyboard_;
		Microsoft::WRL::ComPtr<IDirectInputDevice8> devMouse_;
		std::array<BYTE, 256> key_{};
		std::array<BYTE, 256> keyPre_{};
		MouseState mouseState_{};
		MouseState preMouseState_{};
		XINPUT_STATE xInputState_{};
		XINPUT_STATE preXInputState_{};
		bool isGamePadConnect = false;

	private:
		Input() = default;
		~Input() = default;
		Input(const Input&) = delete;
		const Input& operator=(const Input&) = delete;
	};
}