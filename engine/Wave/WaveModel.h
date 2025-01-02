#pragma once
/**
 * @file WaveModel.h
 * @brief 波が発生するモデル描画
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
#include "Wave/WaveData.h"
#include "Wave/WaveIndexData.h"

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
		kWaveIndexDataNum,
		kWaveParam,

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
		kWaveIndexData,

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
		kTileNum,
		kWaveParam,

		parameterNum
	};

	void Initialize();
	void Finalize();
	//DrawCall前処理
	void PreDraw(PipelineType pipelineType, CommandContext& commandContext, const ViewProjection& viewProjection , const ViewProjection& cullingViewProjection, const TileBasedRendering& tileBasedRendering);

	void Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform,const WaveData& waveData, const WaveIndexData& waveIndexData, const Material& material,const uint32_t textureHandle = 0);
	void Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform, const WaveData& waveData, const WaveIndexData& waveIndexData, SkinCluster& skinCluster, const Material& material, const uint32_t textureHandle = 0);
private: 
	//pipeline生成
	void CreatePipeline();
	//ForwardRendering用pipeline生成
	void CreateForwardPipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_[kPipelineNum];
	std::unique_ptr<PipelineState> pipelineState_[kPipelineNum];

	float hz_ = 0.2f;
	float period_ = 2.0f;
	float amplitude_ = 0.05f;
	float radius_ = 6.0f;
};

