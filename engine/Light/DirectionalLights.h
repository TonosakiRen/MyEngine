#pragma once
/**
 * @file DirectionalLights.h
 * @brief DirectionalLights構造体
 */
#include <d3d12.h>
#include "Mymath.h"
#include "GPUResource/StructuredBuffer.h"
#include "GPUResource/DepthBuffer.h"
#include <vector>

class DirectionalLights
{
public:

	static const uint32_t  lightNum = 1;
	static const uint32_t shadowWidth = 1024;
	static const uint32_t shadowHeight = 1024;


	struct DirectionalLight {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 direction = { 0.0f, -1.0f, 0.0f };
		Vector3 position = { 0.0f,150.0f,0.0f };
		float intensity = 0.075f;
		Vector3 lockUp = { 1.0f,0.0f,0.0f };
		DepthBuffer shadowMap_;
		UploadBuffer constBuffer_;
		uint32_t descriptorHeapIndex;
	};

	struct ConstBufferData {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 direction = { 0.0f, -1.0f, 0.0f };
		float intensity = 0.0f;
		Matrix4x4 viewProjection;
		uint32_t descriptorHeapIndex;
	};

	void Initialize();
	void Update();

	//影用viewProjection
	float fovAngleY_ = 45.0f * std::numbers::pi_v <float> / 180.0f;
	float aspectRatio_ = (float)1 / (float)1;
	float nearZ_ = 0.1f;
	float farZ_ = 300.0f;

public:
	std::vector<DirectionalLight> lights_;
	StructuredBuffer structureBuffer_;

};
