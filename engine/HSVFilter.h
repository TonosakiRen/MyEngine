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
#include "MyMath.h"



class DirectXCommon;

class HSVFilter
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

	Vector3 hsv_ = {0.0f,0.0f,0.0f};
};

