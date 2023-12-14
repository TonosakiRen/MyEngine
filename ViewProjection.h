#pragma once
#include <d3d12.h>
#include "Mymath.h"
#include <numbers>
#include "UploadBuffer.h"

class ViewProjection {
public:

	struct ConstBufferData {
		Matrix4x4 view;
		Matrix4x4 projection;
		Vector3 viewPosition;
	};

	void Initialize();
	void DebugMove();
	void Update();
	bool Shake(Vector3 shakeValue, int& frame);

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return constBuffer_.GetGPUVirtualAddress();
	}

	void SetFarZ(float farZ) {
		farZ_ = farZ;
	}

	const Matrix4x4 GetMatView() const {
		return matView;
	}
public:
	Vector3 translation_ = { 0.0f, 0.0f, -10.0f };
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };
	Vector3 shakeValue_ = { 0.0f,0.0f,0.0f };

	float orthographicValue_ = 1.0f;
private:
	float fovAngleY_ = 45.0f * std::numbers::pi_v <float> / 180.0f;
	float aspectRatio_ = (float)16 / (float)9;
	float nearZ_ = 5.0f;
	float farZ_ = 100.0f;

	Matrix4x4 matView;
	Matrix4x4 matProjection;

	UploadBuffer constBuffer_;
};