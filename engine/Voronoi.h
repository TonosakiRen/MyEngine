#pragma once
#include <stdint.h>
#include "ColorBuffer.h"
#include "CubeColorBuffer.h"
#include "UploadBuffer.h"
#include "DefaultStructuredBuffer.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include <Windows.h>
#include <d3d12.h>

#include <memory>
#include <vector>
#include "Mymath.h"

class CommandContext;

class Voronoi
{
public:

	Voronoi();

	struct VertexData {
		Vector4 pos;
		Vector2 uv;
	};

	struct Point {
		Vector2 point;
		Vector4 color;
		uint32_t index;
	};

	void Initialize(uint32_t pointNum, CommandContext& commandContext);
	void Render(CommandContext& commandContext);

	ColorBuffer& GetResult() { return voronoiTexture_; }
private:
	void InitializeGraphicsPipeline();
private:
	std::unique_ptr<RootSignature> sRootSignature;
	std::unique_ptr<PipelineState> sPipelineState;

	DefaultStructuredBuffer point_;

	Voronoi(const Voronoi&) = delete;
	Voronoi& operator=(const Voronoi&) = delete;

	ColorBuffer voronoiTexture_;


	uint32_t pointNum_ = 0;
};

