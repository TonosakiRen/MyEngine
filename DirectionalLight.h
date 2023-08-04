#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>
#include "Mymath.h"

//定数バッファ用データ構造体
struct ConstBufferDataDirectionalLight {
	Vector4 color;     //ライトの色
	Vector3 direction; //ライト向き
	float intensity;   //輝度
};

/// <summary>
/// ワールド変換データ
/// </summary>
struct DirectionalLight
{
	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	//マッピング済みアドレス
	ConstBufferDataDirectionalLight* constMap = nullptr;
	//ローカルスケール
	Vector4 color_ = { 1.0f, 1.0f, 1.0f,1.0f };
	Vector3 direction_ = { 1.0f, -1.0f, 1.0f };
	float intensity_ = 1.0f;

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
	void UpdateDirectionalLight();
};

