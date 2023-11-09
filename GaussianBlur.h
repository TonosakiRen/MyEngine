#pragma once
#include <stdint.h>
#include "ColorBuffer.h"
#include "UploadBuffer.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include <Windows.h>
#include <d3d12.h>
#include "DirectXCommon.h"

#include <memory>
class GaussianBlur
{
public:
	static uint32_t gbInstanceCount;
	static void StaticInitialize();

	void Initialize(ColorBuffer* originalTexture);
	void Render(ID3D12GraphicsCommandList* commandList);
	void UpdateWeightTable(float blurPower);

	ColorBuffer& GetResult() { return verticalBlurTexture_; }
private:
	static void InitializeGraphicsPipeline();
private:
	static DirectXCommon* sDirectXCommon;
	static ID3D12GraphicsCommandList* sCommandList;
	static std::unique_ptr<RootSignature> sRootSignature;
	static std::unique_ptr<PipelineState> sHorizontalBlurPipelineState;
	static std::unique_ptr<PipelineState> sVerticalBlurPipelineState;

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

