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

	void Initialize(ColorBuffer& orinalBuffer);

	void Draw(ColorBuffer& originalBuffer, CommandContext& commandContext);

	ColorBuffer& GetResult() {
		return resultBuffer_;
	}

private:
	void CreatePipeline();
private:
	RootSignature rootSignature_;
	PipelineState pipelineState_;

	ColorBuffer resultBuffer_;

	float t_ = 0.5f;
};

