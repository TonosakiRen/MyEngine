#pragma once
/**
 * @file GaussianBlur.h
 * @brief GaussianBlurのPipeline
 */
#include <stdint.h>
#include "GPUResource/ColorBuffer.h"
#include "GPUResource/UploadBuffer.h"
#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include <Windows.h>
#include <d3d12.h>

#include <memory>
#include <vector>
#include "Mymath.h"
class GaussianBlur
{
public:

	struct VertexData {
		Vector4 pos;
		Vector2 uv;
	};

	GaussianBlur();
	~GaussianBlur();

	static uint32_t gbInstanceCount;

	void Initialize(ColorBuffer* originalTexture);
	void Render(CommandContext& commandContext);
	//ぼかし度合い更新
	void UpdateWeightTable(float blurPower);

	ColorBuffer& GetResult() { return verticalBlurTexture_; }
private:
	void InitializeGraphicsPipeline();
private:
	static ID3D12GraphicsCommandList* sCommandList;
	std::unique_ptr<RootSignature> sRootSignature;
	std::unique_ptr<PipelineState> sHorizontalBlurPipelineState;
	std::unique_ptr<PipelineState> sVerticalBlurPipelineState;

	static const uint32_t kNumWeights = 8;

	GaussianBlur(const GaussianBlur&) = delete;
	GaussianBlur& operator=(const GaussianBlur&) = delete;


	ColorBuffer* originalTexture_ = nullptr;
	ColorBuffer horizontalBlurTexture_;
	ColorBuffer verticalBlurTexture_;
	UploadBuffer constantBuffer_;
	float weights_[kNumWeights]{};

	PipelineState horizontalBlurPSO;
	PipelineState verticalBlurPSO;

};

