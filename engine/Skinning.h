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
#include "Material.h"

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

	void Initialize();
	void Finalize();
	void PreDraw(CommandContext& commandContext, const ViewProjection& viewProjection);

	void Draw(CommandContext& commandContext,const uint32_t modelHandle, const WorldTransform& worldTransform,const Material& material, const SkinCluster& skinCluster);

private: 
	void CreatePipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;
};

