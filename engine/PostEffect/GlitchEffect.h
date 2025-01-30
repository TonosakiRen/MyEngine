#pragma once
/**
 * @file GlitchEffect.h
 * @brief GlitchEffectのPipeline
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

class GlitchEffect
{
public:
	enum class RootParameter {
		kTexture,
		kData,

		ParameterNum
	};

	void Initialize(ColorBuffer& orinalBuffer);

	void Draw(ColorBuffer& originalBuffer, ColorBuffer& tmpBuffer, CommandContext& commandContext);

private:
	//pipeline生成
	void CreatePipeline(ColorBuffer& orignalBuffer);
private:
	RootSignature rootSignature_;
	PipelineState pipelineState_;

	float intensity_ = 0.2f;
	float speed_ = 10.0f;
	float scale_ = 30.0f;

};

