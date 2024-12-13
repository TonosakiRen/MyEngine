#pragma once
/**
 * @file SpriteData.h
 * @brief Sprite描画用のデータ
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

class SpriteData {
public:
	struct VertexData {
		Vector3 pos;
		Vector2 uv;
	};

	struct ConstBufferData {
		Vector4 color; 
		Matrix4x4 mat; 
	};

	void Initialize(uint32_t textureHandle = 0, Vector2 position = {0.0f,0.0f}, Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f}, Vector2 anchorpoint = {0.5f, 0.5f}, bool isFlipX = false, bool isFlipY = false);

	void SetTextureHandle(uint32_t textureHandle);
	uint32_t GetTextureHandle() { return textureHandle_; }
	//頂点データ更新
	void TransferVertices();

public:

	float rotation_ = 0.0f;
	Vector2 position_{};
	Vector2 size_ = { 100.0f, 100.0f };
	Vector2 anchorPoint_ = { 0.0f, 0.0f };
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	bool isFlipX_ = false;
	bool isFlipY_ = false;
	Vector2 texBase_ = { 0, 0 };
	Vector2 texSize_ = { 100.0f, 100.0f };


	UploadBuffer vertexBuffer_;
	UploadBuffer constBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbView_[2];
	UINT textureHandle_ = 0;
	Matrix4x4 matWorld_{};
	D3D12_RESOURCE_DESC resourceDesc_;
};
