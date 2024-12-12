#pragma once
#include <d3d12.h>
#include "Mymath.h"
#include "GPUResource/StructuredBuffer.h"
#include <array>
#include "GameComponent/WorldTransform.h"

struct PointLight {
	Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
	WorldTransform worldTransform;
	float intensity = 1.0f;
	float radius = 1.0f;
	float decay = 1.0f;
	bool isActive = false;
};
class PointLights
{
public:

	//4の倍数
	static const uint32_t  lightNum = 1024;


	struct ConstBufferData {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 position = { 0.0f, -1.0f, 0.0f };
		float intensity = 1.0f;
		float radius = 1.0f;
		float decay = 1.0f;
		float isActive = false;
	};

	void Initialize();
	void Update();

public:
	std::array<PointLight, lightNum> lights_;
	StructuredBuffer structureBuffer_;
private:

};

