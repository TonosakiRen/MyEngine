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

class Particle
{
public:


	enum class RootParameter {
		kWorldTransform,
		kViewProjection,
		kTexture,
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
	void PreDraw(CommandContext& commandContext, const ViewProjection& viewProjection);

	void Draw(CommandContext& commandContext, ParticleData& bufferData, const Material& material , const uint32_t textureHadle = 0);
	void CreateMesh();
	void CreatePipeline();
	void CreateForwardPipeline();

private:

private:
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;

	std::unique_ptr<RootSignature> fRootSignature_;
	std::unique_ptr<PipelineState> fPipelineState_;

	Matrix4x4 billBordMatrix;

	D3D12_VERTEX_BUFFER_VIEW vbView_;
	D3D12_INDEX_BUFFER_VIEW ibView_;
	std::vector<VertexData> vertices_;
	std::vector<uint16_t> indices_;
	UploadBuffer vertexBuffer_;
	UploadBuffer indexBuffer_;

};