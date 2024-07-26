#pragma once
#include <Windows.h>
#include <memory>
#include <d3d12.h>

#include "PipelineState.h"
#include "RootSignature.h"
#include "UploadBuffer.h"
#include "CommandContext.h"

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DirectionalLights.h"
#include "Material.h"
#include "Mesh.h"
#include "SkinCluster.h"

#include "Renderer.h"
#include "WaveData.h"
#include "WaveIndexData.h"

class DirectXCommon;

class WaveModel
{
public:
	enum class RootParameter {
		kTexture,
		kVertices,
		kMeshlets,
		kUniqueVertexIndices,
		kPrimitiveIndices,
		kCullData,
		kWaveData,
		kDescriptorRangeNum,

		kWorldTransform = kDescriptorRangeNum,
		kViewProjection, 
		kMaterial,
		kMeshletInfo,
		kFrustum,
		kTime,
		kWaveIndexData,

		parameterNum
	};

	enum class ForwardRootParameter {
		kTexture,
		kVertices,
		kMeshlets,
		kUniqueVertexIndices,
		kPrimitiveIndices,
		kCullData,
		kWaveData,

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
		kTime,
		kWaveIndexData,
		kTileNum,

		parameterNum
	};

	void Initialize();
	void Finalize();
	void PreDraw(PipelineType pipelineType, CommandContext& commandContext, const ViewProjection& viewProjection , const ViewProjection& cullingViewProjection, const TileBasedRendering& tileBasedRendering);

	void Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform,const WaveData& waveData, const WaveIndexData& waveIndexData, const Material& material,const uint32_t textureHandle = 0);
	void Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform, const WaveData& waveData, const WaveIndexData& waveIndexData, SkinCluster& skinCluster, const Material& material, const uint32_t textureHandle = 0);
private: 
	void CreatePipeline();
	void CreateForwardPipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_[kPipelineNum];
	std::unique_ptr<PipelineState> pipelineState_[kPipelineNum];
};

