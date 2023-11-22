#pragma once
#include <d3d12.h>
#include "Mymath.h"
#include "UploadBuffer.h"

class DirectionalLight
{
public:
	struct ConstBufferData {
		Vector4 color;
		Vector3 direction;
		float intensity;
	};

	void Initialize();
	void Update();

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return constBuffer_.GetGPUVirtualAddress();
	}
public:
	Vector4 color_ = { 1.0f, 1.0f, 1.0f,1.0f };
	Vector3 direction_ = { 1.0f, -1.0f, 1.0f };
	float intensity_ = 1.0f;
private:
	UploadBuffer constBuffer_;
};

