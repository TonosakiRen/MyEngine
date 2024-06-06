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
#include "Mesh.h"

#include "Voronoi.h"

class DirectXCommon;

class Sky
{
public:
	enum class RootParameter {
		kWorldTransform, 
		kViewProjection, 
		kVoronoiTexture,
		kTopColor,
		kBottomColor,

		parameterNum
	};

	static const uint32_t pointNum_ = 60000;

	void Initialize(CommandContext& commnadContext);
	void Finalize();
	void PreDraw(CommandContext& commandContext, const ViewProjection& viewProjection);

	void Draw(CommandContext& commandContext,const WorldTransform& worldTransform);

private: 
	void CreatePipeline();
	void CreateMesh();
private:
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;

	UploadBuffer vertexBuffer_;
	std::vector<Vector4> vertices_;
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	UploadBuffer indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	std::vector<uint32_t> indices_;

	std::unique_ptr<Voronoi> voronoi_;

	Vector4 topHSVA_ = { 0.790f,1.0f,0.00f,1.0f };;
	Vector4 bottomHSVA_ = { 0.220f,1.0f,0.03f,1.0f };
};

