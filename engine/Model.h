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
#include "DirectionalLights.h"
#include "Material.h"
#include "SkinCluster.h"
#include "Mesh.h"

class DirectXCommon;

class Model
{
public:
	enum class RootParameter {
		kWorldTransform, 
		kViewProjection, 
		kTexture,
		kMaterial,

		parameterNum
	};

	void Initialize();
	void Finalize();
	void PreDraw(CommandContext& commandContext, const ViewProjection& viewProjection);

	void Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform, const Material& material,const uint32_t textureHandle = 0);
	void Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform,SkinCluster& skinCluster, const Material& material, const uint32_t textureHandle = 0);

private: 
	void CreatePipeline();

private:
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;
};

