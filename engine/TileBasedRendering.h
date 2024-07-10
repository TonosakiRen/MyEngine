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

#include "LightManager.h"

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
		uint32_t pointLightIndex[PointLights::lightNum];
		uint32_t spotLightIndex[SpotLights::lightNum];
		uint32_t shadowSpotLightIndex[ShadowSpotLights::lightNum];
	};

	enum class RootParameter {
		kTileInformation,
		kInitialTileFrustum,
		kPointLights,
		kViewProjection,
		ParameterNum
	};

	static const uint32_t kTileWidthNum = 16  * 2;
	static const uint32_t kTileHeightNum = 9  * 2;
	static const uint32_t kTileWidth = WinApp::kWindowWidth / kTileWidthNum;
	static const uint32_t kTileHeight = WinApp::kWindowHeight / kTileHeightNum;
	static const uint32_t kTileNum = kTileWidthNum * kTileHeightNum;

	void Initialize();
	void ComputeUpdate(CommandContext& commandContext,const ViewProjection& viewProjection);


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
	
	std::unique_ptr<DefaultStructuredBuffer> initialTileFrustrumBuffer_;

	std::unique_ptr<RwStructuredBuffer> rwTilesInformation_;

};

