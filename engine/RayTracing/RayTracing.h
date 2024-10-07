#pragma once
#include <Windows.h>
#include <memory>
#include <d3d12.h>

#include "PipelineState.h"
#include "RootSignature.h"
#include "CommandContext.h"

#include "ViewProjection.h"
#include "WorldTransform.h"

#include "Renderer.h"

class DirectXCommon;

class RayTracing
{
public:

	enum class RootParameter {
		kTexture,
		kDescriptorRangeNum,
		kWorldTransform = kDescriptorRangeNum,
		parameterNum
	};
	void Initialize();
	void Finalize();

private:
	void CreatePipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;
};

