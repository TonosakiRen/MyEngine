#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>
#include "Mymath.h"

struct ConstBufferDataWorldTransform {
	Matrix4x4 matWorld; 
};

class WorldTransform
{
public:
	void Initialize();
	void UpdateMatrix();

	void SetParent(WorldTransform* parent) {
		parent_ = parent;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress () const {
		return constBuff_->GetGPUVirtualAddress();
	}
public:
	Vector3 scale_ = { 1.0f,1.0f,1.0f };
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };
	Vector3 translation_ = { 0.0f,0.0f,0.0f };
	Matrix4x4 matWorld_;
private:
	void CreateConstBuffer();
	void Map();
private:
	WorldTransform* parent_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	ConstBufferDataWorldTransform* constMap = nullptr;
};

