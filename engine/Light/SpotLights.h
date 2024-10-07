#pragma once
#include <d3d12.h>
#include "Mymath.h"
#include "StructuredBuffer.h"
#include "DepthBuffer.h"
#include <vector>
#include "WorldTransform.h"

class SpotLights
{
public:

	static const uint32_t  lightNum = 1;

	struct SpotLight {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		WorldTransform worldTransform;
		float intensity = 5.0f;
		Vector3 direction = { 0.0f,-1.0f,0.0f };
		float distance = 18.0f;
		float decay = 0.4f;
		float cosAngle = Radian(45.0f);
		bool isActive = false;
	};

	struct ConstBufferData {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 position = { 0.0f, -1.0f, 0.0f };
		float intensity = 1.0f;
		Vector3 direction = { 1.0f,0.0f,0.0f };
		float distance = 1.0f;
		float decay = 1.0f;
		float cosAngle = Radian(45.0f);
		float isActive = false;
	};

	void Initialize();
	void Update();

public:
	std::vector<SpotLight> lights_;
	StructuredBuffer structureBuffer_;
private:

};

