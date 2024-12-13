#pragma once
/**
 * @file PostEffect.h
 * @brief PostEffectのPipeline
 */
#include <Windows.h>
#include <memory>
#include <vector>
#include <d3d12.h>

#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "Graphics/DescriptorHandle.h"
#include "GPUResource/UploadBuffer.h"

#include "GameComponent/ViewProjection.h"
#include "GameComponent/WorldTransform.h"
#include "Light/DirectionalLights.h"
#include "GameComponent/Material.h"
#include "Mesh/Mesh.h"

class DirectXCommon;

class PostEffect
{
public:
	enum class RootParameter {
		Constant,
		kTexture,

		ParameterNum
	};

	void Initialize();

	void Draw(DescriptorHandle srvHandle, ID3D12GraphicsCommandList* sCommandList);

private:
	void CreatePipeline();
private:
	RootSignature rootSignature_;
	PipelineState pipelineState_;

	float constant_ = 1.0f;
};

