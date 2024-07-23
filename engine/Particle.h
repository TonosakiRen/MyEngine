#pragma once
#include <Windows.h>
#include <memory>
#include <vector>


#include "PipelineState.h"
#include "RootSignature.h"
#include "StructuredBuffer.h"
#include "CommandContext.h"

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DirectionalLights.h"
#include "Material.h"
#include "Mesh.h"

#include "ParticleData.h"

#include "Renderer.h"

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
		kTileInformation,
		kDescriptorRangeNum,

		kViewProjection = kDescriptorRangeNum,
		kMaterial,
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

	void Draw(CommandContext& commandContext, ParticleData& bufferData, const Material& material , const uint32_t textureHadle = 0);
	void CreateMesh();
	void CreatePipeline();
	void CreateForwardPipeline();

private:

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