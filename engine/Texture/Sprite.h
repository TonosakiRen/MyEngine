#pragma once
/**
 * @file Sprite.h
 * @brief Sprite描画用Pipeline
 */
#include <Windows.h>
#include <string>
#include <wrl.h>
#include <memory>
#include "Mymath.h"

#include "Graphics/CommandContext.h"
#include "Graphics/PipelineState.h"
#include "GPUResource/GPUResource.h"
#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "GPUResource/UploadBuffer.h"

#include "Texture/SpriteData.h"

class Sprite {
public:
	struct VertexData {
		Vector3 pos;
		Vector2 uv;
	};

	struct ConstBufferData {
		Vector4 color;
		Matrix4x4 mat;
	};

	void Initialize();
	void Finalize();
	void PreDraw(CommandContext& commandContext);

	void Draw(CommandContext& commandContext, SpriteData& spriteData);

private:
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;
	Matrix4x4 matProjection_;
};