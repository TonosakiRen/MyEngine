#pragma once
#include <d3d12.h>
#include "Mymath.h"
#include <numbers>
#include "UploadBuffer.h"

class ViewProjection {
public:

	static bool isUseDebugCamera;

	struct ConstBufferData {
		Matrix4x4 viewProjection;
		Matrix4x4 inverseViewProjection;
		Matrix4x4 worldMatrix;
		Vector3 viewPosition;
	};

	static void SwitchIsUseDebugCamera();

	void Initialize();
	void Update();

	bool Shake(Vector3 shakeValue, int& frame);

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return constBuffer_.GetGPUVirtualAddress();
	}

	void SetFarZ(float farZ) {
		farZ_ = farZ;
	}

	const Matrix4x4 GetMatView() const {
		return matView_;
	}

	const Matrix4x4 GetMatProjection() const {
		return matProjection_;
	}

	const Matrix4x4 GetInverseViewProjection() const {
		return inverseViewProjection_;
	}

	const Frustum GetFrustum() const {
		return frustum_;
	}

	const Quaternion GetQuaternion() const {
		return quaternion_;
	}

	const Matrix4x4 GetWorldMatrix() const {
		return worldMatrix_;
	}

	void SetTranslation(const Vector3& translation) {
		translation_ = translation;
	}

	void SetQuaternion(const Vector3& euler) {
		quaternion_ = MakeFromEulerAngle(euler);
	}

	void SetQuaternion(const Quaternion& quaternion) {
		quaternion_ = quaternion;
	}

protected:
	Vector3 translation_ = { 0.0f, 7.0f, -27.0f };
	Quaternion quaternion_ = { 0.0f, 0.0f, 0.0f,1.0f };
	Vector3 shakeValue_ = { 0.0f,0.0f,0.0f };

	float orthographicValue_ = 1.0f;

	float fovAngleY_ = 45.0f * std::numbers::pi_v <float> / 180.0f;
	float aspectRatio_ = (float)16 / (float)9;
	float nearZ_ = 0.1f;
	float farZ_ = 100.0f;


	Matrix4x4 worldMatrix_;
	Matrix4x4 matView_;
	Matrix4x4 matProjection_;
	Matrix4x4 inverseViewProjection_;

	Frustum frustum_;

	UploadBuffer constBuffer_;
};