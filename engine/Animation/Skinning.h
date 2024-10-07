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
		kMatrixPalette,
		kInputVertices,
		kInfluences,

		kSkinnedVertices,

		kSkinningInformation,

		parameterNum
	};

	void Initialize();
	void Finalize();

	void PreDispatch(CommandContext& commandContext);
	void Dispatch(CommandContext& commandContext, const uint32_t modelHandle, SkinCluster& skinCluster);
private: 
	void CreatePipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;
};

