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


class DeferredRenderer
{
public:
	enum class RootParameter {
		kColorTexture,
		kNormalTexture,
		kDepthTexture,
		
		kViewProjection,
		kDirectionalLights,

		ParameterNum
	};

	struct VertexData {
		Vector4 pos;
		Vector2 uv;
	};
	void Initialize(ColorBuffer* originalTexture, ColorBuffer* normalTexture, DepthBuffer* depthTexture);
	void Render(CommandContext& commandContext, const ViewProjection& viewProjection, const DirectionalLights& directionalLight);

	ColorBuffer* GetResult() {
		return &resultTexture_;
	}
private:
	void CreatePipeline();
	void CreateMesh();
private:
	static PipelineState pipelineState_;
	static RootSignature rootSignature_;
	ColorBuffer* originalTexture_;
	ColorBuffer* normalTexture_;
	DepthBuffer* depthTexture_;

	ColorBuffer resultTexture_;
	DescriptorHandle uavHandle_;
	

	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	std::vector<VertexData> vertices_;
	std::vector<uint16_t> indices_;
	UploadBuffer vertexBuffer_;
	UploadBuffer indexBuffer_;

	float constant_ = 1.0f;
};

