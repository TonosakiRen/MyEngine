#pragma once
/**
 * @file BlockNoise.h
 * @brief ブロックノイズ用Pipeline
 */
#include <stdint.h>
#include "GPUResource/ColorBuffer.h"
#include "GPUResource/CubeColorBuffer.h"
#include "GPUResource/UploadBuffer.h"
#include "GPUResource/DefaultStructuredBuffer.h"
#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include <Windows.h>
#include <d3d12.h>

#include <memory>
#include <vector>
#include "Mymath.h"

class CommandContext;

class BlockNoise
{
public:

	struct VertexData {
		Vector4 pos;
		Vector2 uv;
	};

	void Initialize();
	void Render(CommandContext& commandContext);

	//Getter
	ColorBuffer& GetResult() { return blockNoiseTexture_; }
private:
	//pipeline生成
	void CreatePipeline();
private:
	std::unique_ptr<RootSignature> sRootSignature;
	std::unique_ptr<PipelineState> sPipelineState;

	ColorBuffer blockNoiseTexture_;

};

