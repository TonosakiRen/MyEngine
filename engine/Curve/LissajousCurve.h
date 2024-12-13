#pragma once
/**
 * @file LissajousCurve.h
 * @brief LissajousCurveを生成
 */
#include <Windows.h>
#include <array>

#include "GPUResource/StructuredBuffer.h"
#include "Graphics/CommandContext.h"
#include "Mymath.h"

class LissajousCurve
{
public:
	static const uint32_t kAllPointNum = 4000;
	static const uint32_t kPointNum = 30;

	void Initialize();
	void Update();

	const DescriptorHandle& GetGPUHandle() const { return structuredBuffer_.GetSRV(); }

private:
	StructuredBuffer structuredBuffer_;
	std::array<Vector2, kAllPointNum> points_;
	uint32_t startPoint_ = kPointNum;
};