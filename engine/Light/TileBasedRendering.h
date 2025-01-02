#pragma once
/**
 * @file TileBasedRendering.h
 * @brief TBR用Pipeline
 */
#include <vector>
#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "GPUResource/UploadBuffer.h"
#include "Graphics/DescriptorHandle.h"
#include "GPUResource/StructuredBuffer.h"
#include "GPUResource/DefaultStructuredBuffer.h"
#include "GPUResource/RwStructuredBuffer.h"
#include "Graphics/WinApp.h"
#include "GameComponent/ViewProjection.h"
class LightNumBuffer;
class TileBasedRendering
{
public:
	static const uint32_t kTileWidthNum = 16;
	static const uint32_t kTileHeightNum = 9 ;
	static const uint32_t kTileWidth = WinApp::kWindowWidth / kTileWidthNum;
	static const uint32_t kTileHeight = WinApp::kWindowHeight / kTileHeightNum;
	static const uint32_t kTileNum = kTileWidthNum * kTileHeightNum;

	static const uint32_t kMaxInTilePointLight = 128;

	void Initialize();
	//実行
	void Dispatch(CommandContext& commandContext, const ViewProjection& viewProjection);

	//Getter
	const DescriptorHandle& GetTileInformationGPUHandle() const {
		return rwTilesInformation_.GetUAV();
	}

	const DescriptorHandle& GetPointLightIndexGPUHandle() const {
		return rwPointLightIndex_.GetUAV();
	}

	const DescriptorHandle& GetSpotLightIndexGPUHandle() const {
		return rwSpotLightIndex_.GetUAV();
	}

	const DescriptorHandle& GetShadowSpotLightIndexGPUHandle() const {
		return rwShadowSpotLightIndex_.GetUAV();
	}

private:

	struct TileInformation {
		uint32_t pointLightNum = 0;
		uint32_t spotLightNum = 0;
		uint32_t shadowSpotLightNum = 0;
		std::vector<uint32_t> pointLightIndex;
		std::vector<uint32_t> spotLightIndex;
		std::vector<uint32_t> shadowSpotLightIndex;
	};

	struct ConstBufferData {
		uint32_t pointLightNum = 0;
		uint32_t spotLightNum = 0;
		uint32_t shadowSpotLightNum = 0;
		uint32_t pointLightOffset = 0;
		uint32_t spotLightOffset = 0;
		uint32_t shadowSpotLightOffset = 0;
	};

	enum class RootParameter {
		kTileInformation,
		kPointLightIndex,
		kSpotLightIndex,
		kShadowSpotLightIndex,
		kInitialTileFrustum,
		kPointLights,
		kSpotLights,
		kShadowSpotLights,
		kLightNum,
		kViewProjection,
		ParameterNum
	};


	void CreatePipeline();
	//初期化視推台
	Frustum GetTileFrustrum(const int& width, const int& height);

private:

	PipelineState pipelineState_;
	RootSignature rootSignature_;

	TileInformation tilesInformation_[kTileNum];

	Frustum initialFrustum_;
	Frustum initialTileFrustrum_[kTileNum];

	Frustum tileFrustrum_[kTileNum];

	const ViewProjection* viewProjection_ = nullptr;

	DefaultStructuredBuffer initialTileFrustrumBuffer_;

	RwStructuredBuffer rwPointLightIndex_;

	RwStructuredBuffer rwSpotLightIndex_;

	RwStructuredBuffer rwShadowSpotLightIndex_;

	RwStructuredBuffer rwTilesInformation_;

};