#pragma once
/**
 * @file Transition.h
 * @brief Transitionを行うPipeline
 */
#include <Windows.h>
#include <memory>
#include <vector>
#include <d3d12.h>

#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "Graphics/DescriptorHandle.h"
#include "GPUResource/UploadBuffer.h"
#include "Mymath.h"

#include "Graphics/CommandContext.h"

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

	//遷移開始
	void StartTransition() {
		isTransition_ = true;
	};
	//遷移できたか
	bool GetIsNextScene() {
		return isNextScene_;
	}
private:
	//pipeline生成
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