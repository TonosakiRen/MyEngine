#pragma once
#include "PipelineState.h"
#include "RootSignature.h"
#include "GPUResource.h"
#include "CommandContext.h"
#include "DescriptorHandle.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
class DeferredRenderer
{
public:
	void Initialize(ColorBuffer* originalTexture);
	void Dispatch(CommandContext& commandContext, ColorBuffer& normalBuffer, DepthBuffer& depthBuffer);
private:
	void CreatePipeline();
private:
	static PipelineState pipelineState_;
	static RootSignature rootSignature_;
	ColorBuffer* originalTexture_;
	ColorBuffer resultTexture_;
	DescriptorHandle uavHandle_;
	int dispatchNum_ = 0;
};

