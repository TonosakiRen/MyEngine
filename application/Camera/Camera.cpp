#include "Camera/Camera.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Easing.h"

void Camera::Initialize()
{
    ViewProjection::Initialize();
}

void Camera::Initialize(Vector3 position, Quaternion quaternion)
{
    ViewProjection::Initialize();
    translation_ = position;
    quaternion_ = quaternion;
}

void Camera::Update(Vector3 playerPos)
{
	Input* input = Input::GetInstance();
	const float rotateSpeed = 0.0000035f / (2.0f * float(M_PI));

	// gamePadが有効なら
	if (!input->PushRightTrigger()) {
		if (input->GetIsGamePadConnect()) {
			float y = input->GetRStick().x * rotateSpeed;
				Quaternion yq = MakeForYAxis(y);
				quaternion_ = yq * quaternion_;
		}
		else {
			if (input->PushKey(DIK_RIGHTARROW)) {
				float y = 30000.0f * rotateSpeed;
					Quaternion yq = MakeForYAxis(y);
					quaternion_ = yq * quaternion_;
			}
			if (input->PushKey(DIK_LEFTARROW)) {
				float y = -30000.0f * rotateSpeed;
				Quaternion yq = MakeForYAxis(y);
				quaternion_ = yq * quaternion_;
			}

			if (input->PushKey(DIK_UPARROW)) {
				float y = -30000.0f * rotateSpeed;
				Quaternion yq = MakeForXAxis(y);
				quaternion_ = yq * quaternion_;
			}

			if (input->PushKey(DIK_DOWNARROW)) {
				float y = 30000.0f * rotateSpeed;
				Quaternion yq = MakeForXAxis(y);
				quaternion_ = yq * quaternion_;
			}
			
		}
	}
	else {
		if (input->PushButton(XINPUT_GAMEPAD_DPAD_RIGHT)) {
			float y = 30000.0f * rotateSpeed;
			Quaternion yq = MakeForYAxis(y);
			quaternion_ = yq * quaternion_;
		}
		if (input->PushButton(XINPUT_GAMEPAD_DPAD_LEFT)) {
			float y = -30000.0f * rotateSpeed;
			Quaternion yq = MakeForYAxis(y);
			quaternion_ = yq * quaternion_;
		}
	}

	interTarget_ = Lerp(0.1f,interTarget_, playerPos);

	//追従対象からカメラまでのオフセット
	Vector3 offset = GetOffset();

	//座標をコピーしてオフセット分ずらす
	 translation_ = interTarget_ + offset;

	 ViewProjection::Update();
}

void Camera::Update()
{
	ViewProjection::Update();
}

Vector3 Camera::GetOffset()
{
	//追従対象からカメラまでのオフセット
	Vector3 offset = { 0.0f,3.2f,-11.0f };

	//カメラの角度から回転行列を計算する
	Quaternion q = MakeYAxisFromQuaternion(quaternion_);

	//オフセットをカメラの回転に合わせて回転させる
	offset = offset * q;
	//座標をコピーしてオフセット分ずらす

	return offset;
}
