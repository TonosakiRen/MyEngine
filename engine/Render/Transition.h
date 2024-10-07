#pragma once
#include <Windows.h>
#include <memory>
#include <vector>
#include <d3d12.h>

#include "PipelineState.h"
#include "RootSignature.h"
#include "DescriptorHandle.h"
#include "UploadBuffer.h"
#include "Mymath.h"

#include "CommandContext.h"

class Transition
{
public:
	enum class RootParameter {
		kT,
		kResult,
		kTexture,

		ParameterNum
	};

	void Initialize(ColorBuffer& resultBuffer);

	void Draw(ColorBuffer& resultBuffer, const DescriptorHandle& textureBuffer, CommandContext& commandContext);

	void StartTransition() {
		isTransition_ = true;
	};

	bool GetIsNextScene() {
		return isNextScene_;
	}

private:
	void CreatePipeline();
private:
	RootSignature rootSignature_;
	PipelineState pipelineState_;
	ColorBuffer saveResultBuffer_;

	float t_ = 0.0f;
	float speed_ = 0.01f;
	bool isNextScene_ = false;
	bool isTransition_ = false;
};