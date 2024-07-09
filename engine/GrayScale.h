#pragma once
#include <Windows.h>
#include <memory>
#include <vector>
#include <d3d12.h>

#include "PipelineState.h"
#include "RootSignature.h"
#include "DescriptorHandle.h"
#include "UploadBuffer.h"
#include "CommandContext.h"



class DirectXCommon;

class GrayScale
{
public:
	enum class RootParameter {
		kTexture,
		kT,

		ParameterNum
	};

	void Initialize(ColorBuffer& originalBuffer);

	void Draw(ColorBuffer& originalBuffer, ColorBuffer& tmpBuffer, CommandContext& commandContext);

private:
	void CreatePipeline(ColorBuffer& originalBuffer);
private:
	RootSignature rootSignature_;
	PipelineState pipelineState_;

	float t_ = 0.0f;
};

