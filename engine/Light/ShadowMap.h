#pragma once
/**
 * @file ShadowMap.h
 * @brief ShadowMapのPipeline
 */
#include <Windows.h>
#include <memory>

#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "Graphics/CommandContext.h"

#include "GameComponent/WorldTransform.h"

class DirectXCommon;
class LightNumBuffer;
class DirectionalLights;

class ShadowMap
{
public:
	enum class RootParameter {
		kWorldTransform,
		kDirectionalLight,

		parameterNum
	};

	void Initialize();
	void Finalize();
	void PreDraw(CommandContext& commandContext, DirectionalLights& directionalLights);

	void Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform);

private:
	void CreatePipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;
	DirectionalLights* directionalLights_;
};

