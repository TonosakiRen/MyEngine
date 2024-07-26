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
#include "Renderer.h"


class DirectXCommon;

class FloorRenderer
{
public:
	enum class RootParameter {
		kCellular,
		kVertices,
		kMeshlets,
		kUniqueVertexIndices,
		kPrimitiveIndices,
		kCullData,
		kDescriptorRangeNum,

		kWorldTransform = kDescriptorRangeNum,
		kViewProjection, 
		kColor,
		kTime,
		kMeshletInfo,
		kFrustum,

		parameterNum
	};

	enum class ForwardRootParameter {
		kCellular,
		kVertices,
		kMeshlets,
		kUniqueVertexIndices,
		kPrimitiveIndices,
		kCullData,
		kDirectionalLights,
		kPointLights,
		kAreaLights,
		kSpotLights,
		kShadowSpotLights,

		kTileInformation,
		kTBRPointLightIndex,
		kTBRSpotLightIndex,
		kTBRShadowSpotLightIndex,

		k2DTextures,
		kDescriptorRangeNum,

		kWorldTransform = kDescriptorRangeNum,
		kViewProjection,
		kColor,
		kTime,
		kMeshletInfo,
		kFrustum,
		kTileNum,

		parameterNum
	};

	void Initialize(CommandContext& commnadContext);
	void Finalize();
	void PreDraw(PipelineType pipelineType, CommandContext& commandContext, const ViewProjection& viewProjection, const ViewProjection& cullingViewProjection,const TileBasedRendering& tileBasedRendering);

	void Draw(CommandContext& commandContext, uint32_t modelHandle,const WorldTransform& worldTransform);

private: 
	void CreatePipeline();
	void CreateForwardPipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_[kPipelineNum];
	std::unique_ptr<PipelineState> pipelineState_[kPipelineNum];

	std::unique_ptr<StructuredBuffer> line_;
	std::unique_ptr<DefaultBuffer> lineNum_;

	std::unique_ptr < Cellular> cellular_;

	Vector4 HSVA_ = { 0.0f,0.2f,0.45f,0.4f };
};

