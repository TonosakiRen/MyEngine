#pragma once
#include <Windows.h>
#include <memory>
#include <vector>
#include <d3d12.h>

#include "PipelineState.h"
#include "RootSignature.h"
#include "DescriptorHandle.h"
#include "UploadBuffer.h"

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DirectionalLights.h"
#include "Material.h"
#include "Mesh.h"

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

