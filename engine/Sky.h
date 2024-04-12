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

	static void StaticInitialize();
	static void Finalize();
	static void PreDraw(CommandContext* commandContext, const ViewProjection& viewProjection);
	static void PostDraw();

	static void Draw(uint32_t modelHandle,const WorldTransform& worldTransform);

private: 
	static void CreatePipeline();
private:
	static CommandContext* commandContext_;
	static std::unique_ptr<RootSignature> rootSignature_;
	static std::unique_ptr<PipelineState> pipelineState_;

	static std::unique_ptr<Voronoi> voronoi_;

	static bool isCreateVoronoi_;
	static Vector4 topHSVA_;
	static Vector4 bottomHSVA_;
};

