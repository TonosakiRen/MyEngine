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

class Reducation
{
public:
	enum class RootParameter {
		kTexture,

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
};

