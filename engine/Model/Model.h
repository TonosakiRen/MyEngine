#pragma once
/**
 * @file Model.h
 * @brief 描画Pipeline
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
#include "Animation/SkinCluster.h"
#include "Mesh/Mesh.h"

#include "Render/Renderer.h"

class DirectXCommon;

class Model
{
public:
	enum class RootParameter {
		kTexture,
		kDescriptorRangeNum,

		kWorldTransform = kDescriptorRangeNum,
		kViewProjection, 
		kMaterial,

		parameterNum
	};

	enum class ForwardRootParameter {
		kTexture,
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
		kTileNum,

		parameterNum
	};

	void Initialize();
	void Finalize();
	void PreDraw(PipelineType pipelineType, CommandContext& commandContext, const ViewProjection& viewProjection,const TileBasedRendering& tileBasedRendering);

	void Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform, const Material& material,const uint32_t textureHandle = 0);
	void Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform,SkinCluster& skinCluster, const Material& material, const uint32_t textureHandle = 0);

private: 
	void CreatePipeline();
	void CreateForwardPipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_[kPipelineNum];
	std::unique_ptr<PipelineState> pipelineState_[kPipelineNum];
};

