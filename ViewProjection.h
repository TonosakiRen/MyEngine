#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>
#include "Mymath.h"
#include <numbers>

//定数バッファ用データ構造体
struct ConstBufferDataViewProjection {
	Matrix4x4 view;			//ワールド->ビュー変換行列
	Matrix4x4 projection;	// ビュー → プロジェクション変換行列
};

/// <summary>
/// ビュープロジェクション変換データ
/// </summary>
struct ViewProjection {
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// マッピング済みアドレス
	ConstBufferDataViewProjection* constMap = nullptr;

#pragma region ビュー行列の設定
	// 視点座標
	Vector3 translation_ = { 0, 0, -10.0f };
	// X,Y,Z軸周りの回転角度
	Vector3 target_ = { 0, 0, 0 };
#pragma endregion

#pragma region 射影行列の設定
	// 垂直方向視野角
	float fovAngleY = 45.0f * std::numbers::pi_v <float> / 180.0f;
	// ビューポートのアスペクト比
	float aspectRatio = (float)16 / (float)9;
	// 深度限界（手前側）
	float nearZ = 0.1f;
	// 深度限界（奥側）
	float farZ = 1000.0f;
#pragma endregion

	// ビュー行列
	Matrix4x4 matView;
	// 射影行列
	Matrix4x4 matProjection;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ID3D12Device* device);
	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer(ID3D12Device* device);
	/// <summary>
	/// マッピングする
	/// </summary>
	void Map();
	/// <summary>
	/// 行列を更新する
	/// </summary>
	void UpdateMatrix();
	/// <summary>
	/// カメラ移動
	/// </summary>
	void DebugMove();
};


