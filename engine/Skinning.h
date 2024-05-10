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

#include "SkinCluster.h"

class DirectXCommon;

class Skinning
{
public:
	enum class RootParameter {
		kWorldTransform,
		kViewProjection,
		kTexture,
		kMaterial,
		kMatrixPalette,

		parameterNum
	};

	static void StaticInitialize();
	static void Finalize();
	static void PreDraw(CommandContext* commandContext, const ViewProjection& viewProjection);
	static void PostDraw();

	static void Draw(uint32_t modelHandle, const WorldTransform& worldTransform, const SkinCluster& skinCluster);

private: 
	static void CreatePipeline();
private:
	static CommandContext* commandContext_;
	static std::unique_ptr<RootSignature> rootSignature_;
	static std::unique_ptr<PipelineState> pipelineState_;
};

