#pragma once
/**
 * @file AreaLights.h
 * @brief AreaLights構造体
 */
#include <d3d12.h>
#include "Mymath.h"
#include "GPUResource/StructuredBuffer.h"
#include <vector>
#include "GameComponent/WorldTransform.h"

struct AreaLight {
	Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
	Segment segment{};
	float intensity = 10.0f;
	float range = 5.0f;
	float decay = 1.7f;
	bool isActive = false;
};

class AreaLights
{
public:

	static const uint32_t  lightNum = 1;

	struct ConstBufferData {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 origin = { 0.0f, 0.0f, 0.0f };
		Vector3 diff = { 0.0f, 1.0f, 0.0f };
		float intensity = 10.0f;
		float range = 5.0f;
		float decay = 1.7f;
		float isActive = 0.0f;
	};

	void Initialize();
	void Update();

public:
	std::vector<AreaLight> lights_;
	StructuredBuffer structureBuffer_;
private:

};

