#pragma once
/**
 * @file Reducation.h
 * @brief 画像をタイリングして増やす
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

class Reducation
{
public:
	enum class RootParameter {
		kTexture,

		ParameterNum
	};

	void Initialize(ColorBuffer& orinalBuffer);

	void Draw(ColorBuffer& originalBuffer, CommandContext& commandContext);
	//結果のGetter
	ColorBuffer& GetResult() {
		return resultBuffer_;
	}

private:
	//pipeline生成
	void CreatePipeline();
private:
	RootSignature rootSignature_;
	PipelineState pipelineState_;

	ColorBuffer resultBuffer_;
};

