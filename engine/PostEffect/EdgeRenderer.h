#pragma once
/**
 * @file EdgeRenderer.h
 * @brief EdgeのPipeline
 */
#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "GPUResource/GPUResource.h"
#include "Graphics/CommandContext.h"
#include "Graphics/DescriptorHandle.h"
#include "GPUResource/ColorBuffer.h"
#include "GPUResource/DepthBuffer.h"
#include "Mymath.h"
#include "GPUResource/UploadBuffer.h"

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
		kEdgeParam,

		ParameterNum
	};

	struct VertexData {
		Vector4 pos;
		Vector2 uv;
	};
	void Initialize(ColorBuffer* colorTexture,ColorBuffer* normalTexture, DepthBuffer* depthTexture);
	//描画
	void Render(CommandContext& commandContext, ColorBuffer& tmpBuffer, ColorBuffer* originalTexture);

private:
	//pipeline生成
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

