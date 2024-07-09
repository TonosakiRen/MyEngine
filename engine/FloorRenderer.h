#pragma once
#include <Windows.h>
#include <memory>
#include <d3d12.h>

#include "PipelineState.h"
#include "RootSignature.h"
#include "UploadBuffer.h"
#include "StructuredBuffer.h"
#include "CommandContext.h"

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DefaultBuffer.h"

#include "Cellular.h"


class DirectXCommon;

class FloorRenderer
{
public:
	enum class RootParameter {
		kCellular,

		kWorldTransform, 
		kViewProjection, 
		kColor,
		kTime,

		parameterNum
	};


	void Initialize(CommandContext& commnadContext);
	void Finalize();
	void PreDraw(CommandContext& commandContext, const ViewProjection& viewProjection);

	void Draw(CommandContext& commandContext, uint32_t modelHandle,const WorldTransform& worldTransform);

private: 
	void CreatePipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;
	std::unique_ptr<StructuredBuffer> line_;
	std::unique_ptr<DefaultBuffer> lineNum_;

	std::unique_ptr < Cellular> cellular_;
	uint32_t time_ = 0;

	Vector4 HSVA_ = { 0.0f,0.2f,0.45f,0.4f };
};

