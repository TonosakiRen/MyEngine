#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>
#include "Mymath.h"

//定数バッファ用データ構造体
struct ConstBufferDataWorldTransform {
	Matrix4x4 matWorld; //ローカル->ワールド　変換行列
};

/// <summary>
/// ワールド変換データ
/// </summary>
struct WorldTransform
{
	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	//マッピング済みアドレス
	ConstBufferDataWorldTransform* constMap = nullptr;
	//ローカルスケール
	Vector3 scale_ = { 1.0f,1.0f,1.0f };
	//X,Y,Z軸周りのローカル回転角
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };
	//ローカル座標
	Vector3 translation_ = { 0.0f,0.0f,0.0f };
	//ローカル->ワールド変換へのポインタ
	Matrix4x4 matWorld_;
	//親となるワールド変換へのポインタ
	WorldTransform* parent_ = nullptr;

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
};

