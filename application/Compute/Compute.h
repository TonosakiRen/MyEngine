#pragma once
#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "GPUResource/GPUResource.h"
#include "Graphics/CommandContext.h"
#include "Graphics/DescriptorHandle.h"
class Compute
{
public:
	void Initialize();
	void Dispatch(CommandContext& commandContext);
	void* GetData();
	void UnMap();
private:
	void CreatePipeline();
private:
	PipelineState pipelineState_;
	RootSignature rootSignature_;
	GPUResource rwStructureBuffer_;
	GPUResource copyBuffer_;
	void* data_;
	const uint32_t kNum = 156;
	DescriptorHandle uavHandle_;
};

