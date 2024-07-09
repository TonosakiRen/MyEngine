#pragma once
#include <stdint.h>
#include "ColorBuffer.h"
#include "CubeColorBuffer.h"
#include "UploadBuffer.h"
#include "StructuredBuffer.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include <Windows.h>
#include <d3d12.h>

#include <memory>
#include <vector>
#include "Mymath.h"

class CommandContext;

class Cellular
{
public:

	Cellular();

	struct Point {
		Vector2 point;
		uint32_t index;
	};

	void Initialize(uint32_t tileWidthNum);
	void Render(CommandContext& commandContext);
	void Finalize();

	ColorBuffer& GetResult() { return *cellularTexture_.get(); }
private:
	void InitializeGraphicsPipeline();
private:
	std::unique_ptr<RootSignature> sRootSignature;
	std::unique_ptr<PipelineState> sPipelineState;

	StructuredBuffer point_;

	Cellular(const Cellular&) = delete;
	Cellular& operator=(const Cellular&) = delete;

	std::unique_ptr<ColorBuffer> cellularTexture_;


	float maxDistance_ = 0;
	float tileWidth_ = 0;
	uint32_t tileWidthNum_ = 0;
};

