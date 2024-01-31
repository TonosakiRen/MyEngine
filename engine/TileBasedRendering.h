#pragma once
#include <vector>
#include "UploadBuffer.h"
#include "DescriptorHandle.h"
#include "PointLights.h"
#include "SpotLights.h"
#include "ShadowSpotLights.h"
#include "StructuredBuffer.h"
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

	static const int kTileWidth = 16;
	static const int kTileHeight = 9;
	static const int kTileNum = kTileWidth * kTileHeight;
	void Initialize();
	void Update(const PointLights& pointLights,const SpotLights& spotLights,const ShadowSpotLights& shadowSpotLights);
public:

	TileInformation tilesInformation_[kTileNum];

	UploadBuffer tileNumBuffer_;

	StructuredBuffer structureBuffer_;

	StructuredBuffer pointLightIndexBuffer_;

	StructuredBuffer spotLightIndexBuffer_;

	StructuredBuffer shadowSpotLightIndexBuffer_;

};

