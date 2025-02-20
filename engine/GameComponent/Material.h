#pragma once
/**
 * @file Material.h
 * @brief Material
 */
#include <d3d12.h>
#include "Mymath.h"
#include "GPUResource/UploadBuffer.h"

class Material
{
public:

	struct ConstBufferData {
		Vector4 color;
		Matrix4x4 uvTransform;
		float intensity;
		uint32_t enableLighting;
	};

	void Initialize();
	void Update();

	//Getter
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return constBuffer_.GetGPUVirtualAddress();
	}
public:
	Vector4 color_ = { 1.0f, 1.0f, 1.0f,1.0f };
	Vector3 scale_ = { 1.0f,1.0f,1.0f };
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };
	Vector3 translation_ = { 0.0f,0.0f,0.0f };
	float intensity_ = 1.0f;
	bool enableLighting_ = true;
private:
	Matrix4x4 uvTransform_;
	UploadBuffer constBuffer_;

};

