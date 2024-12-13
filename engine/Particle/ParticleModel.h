#pragma once
/**
 * @file ParticleModel.h
 * @brief ModelのParticleのPipeline
 */
#include <Windows.h>
#include <memory>
#include <vector>

#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "GPUResource/StructuredBuffer.h"
#include "Graphics/CommandContext.h"

#include "GameComponent/ViewProjection.h"
#include "GameComponent/WorldTransform.h"
#include "Light/DirectionalLights.h"
#include "GameComponent/Material.h"
#include "Mesh/Mesh.h"

#include "Particle/ParticleModelData.h"

#include "Render/Renderer.h"

class DirectXCommon;

class ParticleModel
{
public:

	enum class RootParameter {
		kTexture,
		kParticleData,
		kDescriptorRangeNum,

		kViewProjection = kDescriptorRangeNum,
		kMaterial,
		parameterNum
	};

	enum class ForwardRootParameter {
		kTexture,
		kParticleData,
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

		kViewProjection = kDescriptorRangeNum,
		kMaterial,
		kTileNum,

		parameterNum
	};

	struct VertexData {
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

	void Initialize();
	void Finalize();
	void PreDraw(PipelineType pipelineType, CommandContext& commandContext, const ViewProjection& viewProjection,const TileBasedRendering& tileBasedRendering);

	void Draw(CommandContext& commandContext, ParticleModelData& bufferData, const Material& material, uint32_t modelHandle = 0);

	
private:
	void CreatePipeline();
	void CreateForwardPipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_[kPipelineNum];
	std::unique_ptr<PipelineState> pipelineState_[kPipelineNum];
};