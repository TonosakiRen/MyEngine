#pragma once
#include <vector>
#include "PipelineState.h"
#include "RootSignature.h"
#include "UploadBuffer.h"
#include "DescriptorHandle.h"
#include "StructuredBuffer.h"
#include "DefaultStructuredBuffer.h"
#include "RwStructuredBuffer.h"
#include "WinApp.h"
#include "ViewProjection.h"
class LightNumBuffer;
class TileBasedRendering
{
public:
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
		kLightNum,
		kViewProjection,
		ParameterNum
	};

	static const uint32_t kTileWidthNum = 16;
	static const uint32_t kTileHeightNum = 9 ;
	static const uint32_t kTileWidth = WinApp::kWindowWidth / kTileWidthNum;
	static const uint32_t kTileHeight = WinApp::kWindowHeight / kTileHeightNum;
	static const uint32_t kTileNum = kTileWidthNum * kTileHeightNum;

	static const uint32_t kMaxInTilePointLight = 64;

	void Initialize();
	void ComputeUpdate(CommandContext& commandContext, const ViewProjection& viewProjection);

	void CreatePipeline();
	//初期化視推台
	Frustum GetTileFrustrum(const int& width, const int& height);

public:

	PipelineState pipelineState_;
	RootSignature rootSignature_;

	TileInformation tilesInformation_[kTileNum];

	Frustum initialFrustum_;
	Frustum initialTileFrustrum_[kTileNum];

	Frustum tileFrustrum_[kTileNum];

	const ViewProjection* viewProjection_ = nullptr;

	DefaultStructuredBuffer tileInformationBuffer_;

	DefaultStructuredBuffer pointLightIndexBuffer_;

	StructuredBuffer spotLightIndexBuffer_;

	StructuredBuffer shadowSpotLightIndexBuffer_;



	DefaultStructuredBuffer initialTileFrustrumBuffer_;

	RwStructuredBuffer rwTilesInformation_;

	RwStructuredBuffer rwPointLightIndex_;

	RwStructuredBuffer rwSpotLightIndex_;

	RwStructuredBuffer rwShadowSpotLightIndex_;

};