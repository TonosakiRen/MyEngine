#pragma once
/**
 * @file Skinning.cpp
 * @brief SkinningをするためのPipeline
 */
#include <Windows.h>
#include <memory>
#include <d3d12.h>

#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "GPUResource/UploadBuffer.h"
#include "Graphics/CommandContext.h"
#include "GameComponent/ViewProjection.h"
#include "GameComponent/WorldTransform.h"
#include "Mesh/Mesh.h"
#include "GameComponent/Material.h"
#include "Animation/SkinCluster.h"

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

	//Dispatch準備
	void PreDispatch(CommandContext& commandContext);
	//Dispatch
	void Dispatch(CommandContext& commandContext, const uint32_t modelHandle, SkinCluster& skinCluster);
private: 
	//Pipeline作成
	void CreatePipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;
};

