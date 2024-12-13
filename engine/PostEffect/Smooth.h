#pragma once
/**
 * @file Smooth.h
 * @brief SmoothのPipeline
 */
#include <Windows.h>
#include <memory>
#include <vector>
#include <d3d12.h>

#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "Graphics/DescriptorHandle.h"
#include "GPUResource/UploadBuffer.h"
#include "Graphics/CommandContext.h"



class DirectXCommon;

class Smooth
{
public:
	enum class RootParameter {
		kTexture,
		kT,

		ParameterNum
	};

	void Initialize(ColorBuffer& orinalBuffer);

	void Draw(ColorBuffer& originalBuffer, ColorBuffer& tmpBuffer, CommandContext& commandContext);

private:
	void CreatePipeline(ColorBuffer& originalBuffer);
private:
	RootSignature rootSignature_;
	PipelineState pipelineState_;

	float t_ = 0.0f;
};

