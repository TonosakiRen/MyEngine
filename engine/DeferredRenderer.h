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
class PointLights;
class SpotLights;
class ShadowSpotLights;
class LightNumBuffer;
class TileBasedRendering;

class DeferredRenderer
{
public:
	enum class RootParameter {
		kColorTexture,
		kNormalTexture,
		kDepthTexture,

		kViewProjection,
		kDirectionalLights,
		kPointLights,
		kSpotLights,
		kShadowSpotLights,

		k2DTextures,

		kLightNum,

		kTBRInformation,

		kTBRPointLightIndex,
		kTBRSpotLightIndex,
		kTBRShadowSpotLightIndex,

		ParameterNum
	};

	struct VertexData {
		Vector4 pos;
		Vector2 uv;
	};
	void Initialize(ColorBuffer* originalTexture, ColorBuffer* normalTexture, DepthBuffer* depthTexture);
	void Render(CommandContext& commandContext, ColorBuffer* originalBuffer_, const ViewProjection& viewProjection, DirectionalLights& directionalLight, const PointLights& pointLights, const SpotLights& spotLights, const ShadowSpotLights& shadowSpotLights, const LightNumBuffer& lightNumBuffer, const TileBasedRendering& tileBasedRendering);

private:
	void CreatePipeline();
	void CreateMesh();
private:
	PipelineState pipelineState_;
	RootSignature rootSignature_;
	ColorBuffer* colorTexture_;
	ColorBuffer* normalTexture_;
	DepthBuffer* depthTexture_;

	struct LightNum {
		int32_t directionalLightNum;
		int32_t pointLightNum;
		int32_t spotLightNum;
	};


	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	std::vector<VertexData> vertices_;
	std::vector<uint16_t> indices_;
	UploadBuffer vertexBuffer_;
	UploadBuffer indexBuffer_;


};

