#pragma once
#include <Windows.h>
#include <memory>
#include <d3d12.h>

#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "GPUResource/UploadBuffer.h"
#include "Graphics/CommandContext.h"

#include "GameComponent/ViewProjection.h"
#include "GameComponent/WorldTransform.h"
#include "Light/DirectionalLights.h"
#include "GameComponent/Material.h"
#include "Mesh/Mesh.h"
#include "Animation/SkinCluster.h"
#include "Sky/Sky.h"

#include "Render/Renderer.h"

class DirectXCommon;

class MeshletEnvironmentMapModel
{
public:
	enum class RootParameter {
		kTexture,
		kVertices,
		kMeshlets,
		kUniqueVertexIndices,
		kPrimitiveIndices,
		kCullData,
		kDescriptorRangeNum,

		kWorldTransform = kDescriptorRangeNum,
		kViewProjection, 
		kMaterial,
		kMeshletInfo,
		kFrustum,
		kTopColor,
		kBottomColor,

		parameterNum
	};

	enum class ForwardRootParameter {
		kTexture,
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
		kMaterial,
		kMeshletInfo,
		kFrustum,
		kTileNum,
		kTopColor,
		kBottomColor,


		parameterNum
	};

	void Initialize(Sky& sky);
	void Finalize();
	void PreDraw(PipelineType pipelineType, CommandContext& commandContext, const ViewProjection& viewProjection , const ViewProjection& cullingViewProjection,const TileBasedRendering& tileBasedRendering);

	void Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform, const Material& material);
	void Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform, SkinCluster& skinCluster, const Material& material);
private: 
	void CreatePipeline();
	void CreateForwardPipeline();
private:
	Sky* sky_;
	std::unique_ptr<RootSignature> rootSignature_[kPipelineNum];
	std::unique_ptr<PipelineState> pipelineState_[kPipelineNum];
};

