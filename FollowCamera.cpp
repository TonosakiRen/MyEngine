#include "FollowCamera.h"
#include "Input.h"
#include <numbers>
#include "ImGuiManager.h"
void FollowCamera::Initialize() {
	// ビュープロジェクション
	viewProjection_.Initialize();
}
void FollowCamera::Update() {

	Input* input = Input::GetInstance();
	const float rotateSpeed = 0.0000035f / (2.0f * float(M_PI));
	// gamePadが有効なら
	if (input->GetIsGamePadConnect()) {
		viewProjection_.target_.y += input->GetRStick().x * rotateSpeed;
	}
	else {
		if (input->PushKey(DIK_LEFTARROW)) {
			viewProjection_.target_.y += 30000.0f * rotateSpeed;
		}
		if (input->PushKey(DIK_RIGHTARROW)) {
			viewProjection_.target_.y -= 30000.0f * rotateSpeed;
		}
		
	}

	// 追従対象がいれば
	if (target_) {

		//追従対象からカメラまでのオフセット
		Vector3 offset = { 0.0f, 10.0f, -20.0f };
		viewProjection_.target_.x = 15.0f * std::numbers::pi_v<float> / 180.0f;

		//カメラの角度から回転行列を計算する
		Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewProjection_.target_.y);

		//オフセットをカメラの回転に合わせて回転させる
		offset = offset* rotateMatrix;
		//座標をコピーしてオフセット分ずらす
		viewProjection_.translation_ = MakeTranslation(target_->matWorld_) + offset;
	}

	viewProjection_.UpdateMatrix();
}