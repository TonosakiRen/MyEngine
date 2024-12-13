#pragma once
/**
 * @file SpotLightShadowMap.h
 * @brief SpotLightShadowMapのPipeline
 */
#include <Windows.h>
#include <memory>

#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "Graphics/CommandContext.h"

#include "GameComponent/WorldTransform.h"

class DirectXCommon;
class LightNumBuffer;
class ShadowSpotLights;

class SpotLightShadowMap
{
public:
	enum class RootParameter {
		kWorldTransform,
		kShadowSpotLight,

		parameterNum
	};


	void Initialize();
	void Finalize();
	void PreDraw(CommandContext& commandContext, ShadowSpotLights& shadowSpotLights);

	void Draw(CommandContext& commandContext, uint32_t modelHandle, const WorldTransform& worldTransform);

private:
	void CreatePipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;
	ShadowSpotLights* shadowSpotLights_;
};

