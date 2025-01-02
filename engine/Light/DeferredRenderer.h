#pragma once
/**
 * @file DeferredRenderer.h
 * @brief DeferredRendering用Pipeline
 */
#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "GPUResource/GPUResource.h"
#include "Graphics/CommandContext.h"
#include "Graphics/DescriptorHandle.h"
#include "GPUResource/ColorBuffer.h"
#include "GPUResource/DepthBuffer.h"
#include "Mymath.h"
#include "GPUResource/UploadBuffer.h"

class ViewProjection;
class TileBasedRendering;

class DeferredRenderer
{
public:
	enum class RootParameter {
		kColorTexture,
		kNormalTexture,
		kMaterialTexture,
		kDepthTexture,

		kDirectionalLights,
		kPointLights,
		kAreaLights,
		kSpotLights,
		kShadowSpotLights,

		k2DTextures,


		kTBRInformation,

		kTBRPointLightIndex,
		kTBRSpotLightIndex,
		kTBRShadowSpotLightIndex,

		kDescriptorRageNum,

		kViewProjection = kDescriptorRageNum,
		kLightNum,
		kTileNum,

		ParameterNum
	};

	void Initialize(ColorBuffer* originalTexture, ColorBuffer* normalTexture, ColorBuffer* materialTexture, DepthBuffer* depthTexture);
	//描画
	void Render(CommandContext& commandContext, ColorBuffer* originalBuffer_, ViewProjection& viewProjection, TileBasedRendering& tileBasedRendering);

private:
	//pipeline生成
	void CreatePipeline();
private:
	PipelineState pipelineState_;
	RootSignature rootSignature_;
	ColorBuffer* colorTexture_;
	ColorBuffer* normalTexture_;
	ColorBuffer* materialTexture_;
	DepthBuffer* depthTexture_;

	struct LightNum {
		int32_t directionalLightNum;
		int32_t pointLightNum;
		int32_t spotLightNum;
	};


};