#pragma once
#include <Windows.h>
#include <memory>

#include "PipelineState.h"
#include "RootSignature.h"
#include "CommandContext.h"

#include "WorldTransform.h"

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

