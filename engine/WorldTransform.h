#pragma once
#include <d3d12.h>
#include "Mymath.h"
#include "UploadBuffer.h"

class WorldTransform
{
public:

	struct ConstBufferData {
		Matrix4x4 matWorld;
		Matrix4x4 worldInverseTranspose;
		Vector3 scale;
	};

	//bufferに送る場合の初期化
	void Initialize(bool srtChange = true);

	void Reset();

	void Update();
	void Update(Matrix4x4 localMatrix);
	void Update(uint32_t modelIndex);

	void ConstUpdate() {
		constBuffer_.Copy(matWorld_);
	}

	void SetParent(WorldTransform* parent,bool applyLocalSpace = true) {
		if (applyLocalSpace) {
			if (parent != parent_) {
				if (parent) {
					Matrix4x4 localMatrix = matWorld_ * Inverse(parent->matWorld_);
					translation_ = MakeTranslation(localMatrix);
					if (isRotateParent_ == true) {
						quaternion_ = RotateMatrixToQuaternion((NormalizeMakeRotateMatrix(localMatrix)));
					}
					if (isScaleParent_ == true) {
						scale_ = MakeScale(localMatrix);
					}

					parent_ = parent;
				}
				else {
					translation_ = MakeTranslation(matWorld_);
					if (isRotateParent_ == true) {
						quaternion_ = RotateMatrixToQuaternion((NormalizeMakeRotateMatrix(matWorld_)));
					}
					if (isScaleParent_ == true) {
						scale_ = MakeScale(matWorld_);
					}
					parent_ = parent;
				}
			}
		}
		else {
			parent_ = parent;
		}
	}
	void SetIsScaleParent(bool isScaleParent) {
		isScaleParent_ = isScaleParent;
	}
	void SetIsRotateParent(bool isRotateParent) {
		isRotateParent_ = isRotateParent;
	}
	WorldTransform* GetParent() {
		return parent_;
	}
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return constBuffer_.GetGPUVirtualAddress();
	}
public:
	Vector3 scale_ = { 1.0f,1.0f,1.0f };
	Quaternion quaternion_ = IdentityQuaternion();
	Vector3 translation_ = { 0.0f,0.0f,0.0f };
	Matrix4x4 matWorld_{};
	Matrix4x4 worldInverseTranspose_{};

	float maxScale_ = 1.0f;
private:
	WorldTransform* parent_ = nullptr;
	bool isScaleParent_ = true;
	bool isRotateParent_ = true;

	UploadBuffer constBuffer_;
};