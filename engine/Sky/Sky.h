#pragma once
/**
 * @file Sky.h
 * @brief 空描画のPipeline
 */
#include <Windows.h>
#include <memory>
#include <d3d12.h>

#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "GPUResource/DefaultBuffer.h"
#include "Graphics/CommandContext.h"

#include "GameComponent/ViewProjection.h"
#include "GameComponent/WorldTransform.h"
#include "Mesh/Mesh.h"

#include "Sky/Voronoi.h"
#include "PostEffect/Reducation.h"

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
	//Drawcall描画前処理
	void PreDraw(CommandContext& commandContext, const ViewProjection& viewProjection);
	//Drawcall
	void Draw(CommandContext& commandContext,const WorldTransform& worldTransform);

	//Getter
	CubeColorBuffer& GetStarTexture() { return starTexture_; }
	const Vector4& GetTopColor() { return topHSVA_; }
	const Vector4& GetBottomColor() { return bottomHSVA_; }
private: 
	//pipeline生成
	void CreatePipeline();
	//mesh生成
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

	Vector4 topHSVA_ = { 0.650f,1.0f,0.03f,1.0f };;
	Vector4 bottomHSVA_ = { 0.550f,1.0f,0.0f,1.0f };
};

