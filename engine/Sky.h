#pragma once
#include <Windows.h>
#include <memory>
#include <d3d12.h>

#include "PipelineState.h"
#include "RootSignature.h"
#include "DefaultBuffer.h"
#include "CommandContext.h"

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Mesh.h"

#include "Voronoi.h"
#include "Reducation.h"

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

	CubeColorBuffer& GetStarTexture() { return starTexture_; }
	const Vector4& GetTopColor() { return topHSVA_; }
	const Vector4& GetBottomColor() { return bottomHSVA_; }
private: 
	void CreatePipeline();
	void CreateMesh(CommandContext& commnadContext);
private:
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;

	DefaultBuffer vertexBuffer_;
	std::vector<Vector4> vertices_;
	D3D12_VERTEX_BUFFER_VIEW vbView_{};


	std::unique_ptr<Voronoi> voronoi_;

	std::unique_ptr<Reducation> reducation_;

	CubeColorBuffer starTexture_;

	Vector4 topHSVA_ = { 0.650f,1.0f,0.07f,1.0f };;
	Vector4 bottomHSVA_ = { 0.550f,1.0f,0.15f,1.0f };
};

