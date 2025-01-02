#pragma once
/**
 * @file MeshletModel.h
 * @brief Meshletでの描画Pipeline
 */
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

#include "Render/Renderer.h"

class DirectXCommon;

class MeshletModel
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

		parameterNum
	};

	void Initialize();
	void Finalize();
	//DrawCall実行前処理
	void PreDraw(PipelineType pipelineType, CommandContext& commandContext, const ViewProjection& viewProjection , const ViewProjection& cullingViewProjection,const TileBasedRendering& tileBasedRendering);
	//DrawCall
	void Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform, const Material& material,const uint32_t textureHandle = 0);
	void Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform, SkinCluster& skinCluster, const Material& material, const uint32_t textureHandle = 0);
private: 
	//pipeline生成
	void CreatePipeline();
	//ForwardRenderingPipeline生成
	void CreateForwardPipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_[kPipelineNum];
	std::unique_ptr<PipelineState> pipelineState_[kPipelineNum];
};

