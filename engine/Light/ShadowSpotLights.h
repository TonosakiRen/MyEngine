#pragma once
/**
 * @file ShadowSpotLights.h
 * @brief ShadowSpotLights構造体
 */
#include <d3d12.h>
#include "Mymath.h"
#include "GPUResource/StructuredBuffer.h"
#include "GPUResource/DepthBuffer.h"
#include <vector>
#include "GameComponent/WorldTransform.h"

struct ShadowSpotLight {
	Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
	WorldTransform worldTransform;
	float intensity = 1.0f;
	Vector3 direction = { 0.0f,-1.0f,0.0f };
	float distance = 1.0f;
	float decay = 1.0f;
	float cosAngle = Radian(45.0f);
	bool isActive = false;
	Vector3 lockUp = { 1.0f,0.0f,0.0f };
	DepthBuffer shadowMap_;
	UploadBuffer constBuffer_;
	uint32_t descriptorHeapIndex;
};

class ShadowSpotLights
{
public:

	static const uint32_t  lightNum = 1;
	static const uint32_t shadowWidth = 1024;
	static const uint32_t shadowHeight = 1024;


	struct ConstBufferData {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 position = { 0.0f, -1.0f, 0.0f };
		float intensity = 1.0f;
		Vector3 direction = { 1.0f,0.0f,0.0f };
		float distance = 1.0f;
		float decay = 1.0f;
		float cosAngle = Radian(45.0f);
		float isActive = false;
		uint32_t descriptorHeapIndex;
		Matrix4x4 viewProjection;
	};

	void Initialize();
	void Update();

public:
	std::vector<ShadowSpotLight> lights_;
	StructuredBuffer structureBuffer_;
private:
	//影用viewProjection
	float fovAngleY_ = 45.0f * std::numbers::pi_v <float> / 180.0f;
	float aspectRatio_ = (float)1 / (float)1;
	float nearZ_ = 0.1f;
	float farZ_ = 300.0f;
};

