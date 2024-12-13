#pragma once
/**
 * @file Raytracing.h
 * @brief RaytracingのPipeline
 */
#include <Windows.h>
#include <memory>
#include <d3d12.h>

#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "Graphics/CommandContext.h"

#include "GameComponent/ViewProjection.h"
#include "GameComponent/WorldTransform.h"
#include "Raytracing/TLAS.h"

#include "Render/Renderer.h"
#include "Raytracing/RaytracingInstanceDescs.h"
#include "Raytracing/BLAS.h"

class DirectXCommon;

class Raytracing
{
public:

	enum class RootParameter {
		kResult,
		kTras,
		kDescriptorRangeNum,

		parameterNum = kDescriptorRangeNum
	};

	void Initialize();
	void Update(CommandContext& commandContext);
	void AddInstanceDesc(const BLAS& blas, const WorldTransform& worldTransform);
	void AddInstanceDesc(const BLAS& blas, const Matrix4x4& matrix);
	void Finalize();

private:
	void CreatePipeline();
private:
	std::unique_ptr<TLAS> tras_;
	std::unique_ptr<RaytracingInstanceDescs> instanceDescs_;
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;
};

