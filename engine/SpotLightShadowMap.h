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

	static bool isDrawSpotLightShadowMap;

	static void StaticInitialize();
	static void Finalize();
	static void PreDraw(CommandContext* commandContext, ShadowSpotLights& shadowSpotLights);
	static void PostDraw();

	static void Draw(uint32_t modelHandle, const WorldTransform& worldTransform);

private:
	static void CreatePipeline();
private:
	static CommandContext* commandContext_;
	static std::unique_ptr<RootSignature> rootSignature_;
	static std::unique_ptr<PipelineState> pipelineState_;
	static ShadowSpotLights* shadowSpotLights_;
};

