#pragma once
/**
 * @file Particle.h
 * @brief ParticleのPipeline
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

#include "Particle/ParticleData.h"

#include "Render/Renderer.h"

class Particle
{
public:


	enum class RootParameter {
		kTexture,
		kParticleData,
		kDescriptorRangeNum,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                		kDescriptorRange,

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
	//DrawCall実行前処理
	void PreDraw(PipelineType pipelineType, CommandContext& commandContext, const ViewProjection& viewProjection,const TileBasedRendering& tileBasedRendering);
	//DrawCall
	void Draw(CommandContext& commandContext, ParticleData& bufferData, const Material& material , const uint32_t textureHadle = 0);

private:
	//Mesh生成
	void CreateMesh();
	//pipeline生成
	void CreatePipeline();
	//ForwardRendering用Pipeline生成
	void CreateForwardPipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_[kPipelineNum];
	std::unique_ptr<PipelineState> pipelineState_[kPipelineNum];

	Matrix4x4 billBordMatrix;

	D3D12_VERTEX_BUFFER_VIEW vbView_;
	D3D12_INDEX_BUFFER_VIEW ibView_;
	std::vector<VertexData> vertices_;
	std::vector<uint16_t> indices_;
	UploadBuffer vertexBuffer_;
	UploadBuffer indexBuffer_;

};