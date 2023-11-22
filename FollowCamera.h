#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Mymath.h"
#include "Player.h"
#include "LockOn.h"
/// <summary>
/// 追従カメラ
/// </summary>
class FollowCamera {
public:

	//調整項目の適用
	void ApplyGlobalVariables();
	float delayCameraSpeed_ = 0.1f;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ビュープロジェクションを取得
	/// </summary>
	/// <returns>ビュープロジェクション</returns>
	ViewProjection& GetViewProjection() { return viewProjection_; }

	void Reset();

	Vector3 GetOffset()const;

	void SetTarget(const WorldTransform* target);

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

private:


	//追従対象
	const WorldTransform* target_ = nullptr;
	//追従対象向き
	Vector3 direction_;
	//ビュープロジェクション
	ViewProjection viewProjection_;
	//追従対象の向きに移動
	bool moveDirection_ = false;

	float a = 0.0f;

	bool isFocus = false;
	Vector3 offset_ = { 4.0f,4.0f,-10.0f };
	float focusMoveT = 3.0f;
	float rataionspeed = 0.057f;
	float movespeed = 0.90f;

	// 結果
	Vector3 goalRotation = { 0.0f, 0.0f, 0.0f };
	Vector3 goalTranslation = { 0.0f, 0.0f, 0.0f };

	Vector3 interTarget_ = {};
	float destinationAngleY_;

	const LockOn* lockOn_ = nullptr;
};
