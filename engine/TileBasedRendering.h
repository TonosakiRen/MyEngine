#pragma once
#include <vector>
#include "UploadBuffer.h"
#include "DescriptorHandle.h"
#include "PointLights.h"
#include "SpotLights.h"
class TileBasedRendering
{
public:
	struct TileInformation {
		uint32_t pointLightNum = 0;
		uint32_t spotLightNum = 0;
		std::vector<uint32_t> pointLightIndex;
		std::vector<uint32_t> spotLightIndex;
	};

	struct ConstBufferData {
		uint32_t pointLightNum = 0;
		uint32_t spotLightNum = 0;
		uint32_t pointLightIndex[PointLights::lightNum];
	};
	static const int kTileWidth = 16;
	static const int kTileHeight = 9;
	static const int kTileNum = kTileWidth * kTileHeight;
	void Initialize();
	void Update();
public:
	TileInformation tilesInformation_[kTileNum];
	UploadBuffer structureBuffer_;
	DescriptorHandle srvHandle_;
};

