#pragma once
#include <vector>
#include "PipelineState.h"
#include "RootSignature.h"
#include "UploadBuffer.h"
#include "DescriptorHandle.h"
#include "PointLights.h"
#include "SpotLights.h"
#include "ShadowSpotLights.h"
#include "StructuredBuffer.h"
#include "RwStructuredBuffer.h"
#include "WinApp.h"
#include "ViewProjection.h"
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
	};

	enum class RootParameter {
		kTileInformation,
		kPointLightIndex,
		kSpotLightIndex,
		kShadowSpotLightIndex,
		kInitialTileFrustum,
		ParameterNum
	};

	static const uint32_t kTileWidthNum = 16 ;
	static const uint32_t kTileHeightNum = 9 ;
	static const uint32_t kTileWidth = WinApp::kWindowWidth / kTileWidthNum;
	static const uint32_t kTileHeight = WinApp::kWindowHeight / kTileHeightNum;
	static const uint32_t kTileNum = kTileWidthNum * kTileHeightNum;

	void Initialize();
	void Update(const ViewProjection& viewProjection,const PointLights& pointLights,const SpotLights& spotLights,const ShadowSpotLights& shadowSpotLights);
	void ComputeUpdate(CommandContext& commandContext,const ViewProjection& viewProjection, const PointLights& pointLights, const SpotLights& spotLights, const ShadowSpotLights& shadowSpotLights);


	void CreatePipeline();
	//初期化視推台
	Frustum GetTileFrustrum(const int& width, const int& height);

	//当たり判定
	bool IsHitSphere(const Frustum& frustum,const Vector3& position,const float radius);

public:

	PipelineState pipelineState_;
	RootSignature rootSignature_;

	TileInformation tilesInformation_[kTileNum];

	Frustum initialFrustum_;
	Frustum initialTileFrustrum_[kTileNum];

	Frustum tileFrustrum_[kTileNum];

	const ViewProjection* viewProjection_ = nullptr;

	StructuredBuffer tileInformationBuffer_;

	StructuredBuffer pointLightIndexBuffer_;

	StructuredBuffer spotLightIndexBuffer_;

	StructuredBuffer shadowSpotLightIndexBuffer_;


	
	DefaultStructuredBuffer initialTileFrustrumBuffer_;

	RwStructuredBuffer rwTilesInformation_;

	UploadBuffer resetTileInformation_;

	RwStructuredBuffer rwPointLightIndex_;

	RwStructuredBuffer rwSpotLightIndex_;

	RwStructuredBuffer rwShadowSpotLightIndex_;

};

