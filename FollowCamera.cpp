#include "FollowCamera.h"
#include "Input.h"
#include <numbers>
#include "ImGuiManager.h"
#include "GlobalVariables.h"
#include "Easing.h"

void FollowCamera::ApplyGlobalVariables() {

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "FollowCamera";
	delayCameraSpeed_ = globalVariables->GetFloatValue(groupName, "delayCameraSpeed");
}

void FollowCamera::Initialize() {
	// ビュープロジェクション
	viewProjection_.Initialize();

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "FollowCamera";
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "delayCameraSpeed", delayCameraSpeed_);

	ApplyGlobalVariables();
}
void FollowCamera::Update() {

	ApplyGlobalVariables();

	delayCameraSpeed_ = clamp(delayCameraSpeed_, 0.0f, 1.0f);

	if (lockOn_->ExitTarget()) {
		Vector3 lockOnPosition = lockOn_->GetTargetPosition();
		Vector3 sub = lockOnPosition - MakeTranslation(target_->matWorld_);
		sub = { sub.x,0.0f,sub.z };
		sub = Normalize(sub);

		/*viewProjection_.target_.y = std::atan2(sub.x, sub.z);*/


		float angle = Angle({ 0.0f,0.0f,1.0f }, sub);

		

		if (Cross({ 0.0f,1.0f }, Vector2{ sub.x,sub.z }) < 0.0f) {

			viewProjection_.target_.y = Easing::easing(0.02f, viewProjection_.target_.y, angle);
		}
		else {
			viewProjection_.target_.y = Easing::easing(0.02f, viewProjection_.target_.y, -angle);
		}

		
	}
	else {
		Input* input = Input::GetInstance();
		const float rotateSpeed = 0.0000035f / (2.0f * float(M_PI));
		// gamePadが有効なら
		if (input->GetIsGamePadConnect()) {
			viewProjection_.target_.y += input->GetRStick().x * rotateSpeed;
		}
		else {
			if (input->PushKey(DIK_RIGHTARROW)) {
				viewProjection_.target_.y += 30000.0f * rotateSpeed;
			}
			if (input->PushKey(DIK_LEFTARROW)) {
				viewProjection_.target_.y -= 30000.0f * rotateSpeed;
			}

		}
	}

	// 追従対象がいれば
	if (target_) {

		interTarget_ = Lerp(interTarget_, MakeTranslation(target_->matWorld_), delayCameraSpeed_);

		//追従対象からカメラまでのオフセット
		Vector3 offset = GetOffset();
		viewProjection_.target_.x = 15.0f * std::numbers::pi_v<float> / 180.0f;

		//座標をコピーしてオフセット分ずらす
		viewProjection_.translation_ = interTarget_ + offset;
	}

	viewProjection_.UpdateMatrix();
}

void FollowCamera::Reset()
{
	if (target_) {
		interTarget_ = MakeTranslation(target_->matWorld_);
		viewProjection_.target_.y = MakeEulerAngle(target_->matWorld_).y;
	}
	destinationAngleY_ = viewProjection_.target_.y;

	Vector3 offset = GetOffset();
	viewProjection_.translation_ = interTarget_ + offset;
}

Vector3 FollowCamera::GetOffset() const
{
	//追従対象からカメラまでのオフセット
	Vector3 offset = { 0.0f, 10.0f, -20.0f };

	//カメラの角度から回転行列を計算する
	Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewProjection_.target_.y);

	//オフセットをカメラの回転に合わせて回転させる
	offset = offset * rotateMatrix;
	//座標をコピーしてオフセット分ずらす

	return offset;
}

void FollowCamera::SetTarget(const WorldTransform* target)
{
	target_ = target;
	Reset();
}
