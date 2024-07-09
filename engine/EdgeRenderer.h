#pragma once
#include "PipelineState.h"
#include "RootSignature.h"
#include "GPUResource.h"
#include "CommandContext.h"
#include "DescriptorHandle.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "Mymath.h"
#include "UploadBuffer.h"

class ViewProjection;
class DirectionalLights;


class EdgeRenderer
{
public:
	enum class RootParameter {
		kColorTexture,
		kNormalTexture,
		kDepthTexture,
		kEdgeColor,

		ParameterNum
	};

	struct VertexData {
		Vector4 pos;
		Vector2 uv;
	};
	void Initialize(ColorBuffer* colorTexture,ColorBuffer* normalTexture, DepthBuffer* depthTexture);
	void Render(CommandContext& commandContext, ColorBuffer& tmpBuffer, ColorBuffer* originalTexture);

private:
	void CreatePipeline();
private:
	PipelineState edgePipelineState_;
	RootSignature edgeRootSignature_;

	ColorBuffer* colorTexture_;
	ColorBuffer* normalTexture_;
	DepthBuffer* depthTexture_;

	DescriptorHandle uavHandle_;

	Vector3 edgeColor_ = { 0.0f,0.0f,0.0f };
	float normalThreshold_ = 0.2f;
	float depthThreshold_ = 0.001f;

};

