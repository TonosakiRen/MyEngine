/**
 * @file SpriteData.cpp
 * @brief Sprite描画用のデータ
 */
#include "Texture/SpriteData.h"
#include "Texture/TextureManager.h"
#include <cassert>
#include "Graphics/WinApp.h"
#include "Light/ShaderManager.h"
#include "Render/Renderer.h"

using namespace Microsoft::WRL;

void SpriteData::Initialize(uint32_t textureHandle, Vector2 position, Vector4 color, Vector2 anchorpoint, bool isFlipX, bool isFlipY) {

	Vector2 size = { 100.0f, 100.0f };

	const D3D12_RESOURCE_DESC& resDesc = Engine::TextureManager::GetInstance()->GetResoureDesc(textureHandle);
	size = { (float)resDesc.Width, (float)resDesc.Height };

	position_ = position;
	size_ = size;
	anchorPoint_ = anchorpoint;
	matWorld_ = MakeIdentity4x4();
	color_ = color;
	textureHandle_ = textureHandle;
	isFlipX_ = isFlipX;
	isFlipY_ = isFlipY;
	texSize_ = size;

	resourceDesc_ = Engine::TextureManager::GetInstance()->GetResoureDesc(textureHandle_);

	UINT bufferSize = sizeof(VertexData) * 4;

	vertexBuffer_.Create(L"spriteVertexBuffer", bufferSize);

	// 頂点バッファへのデータ転送
	TransferVertices();

	// 頂点バッファビューの作成
	vbView_[0].BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbView_[0].SizeInBytes = bufferSize;
	vbView_[0].StrideInBytes = sizeof(VertexData);

	size_t offset = Helper::AlignUp(bufferSize, 256);
	vbView_[1].BufferLocation = vertexBuffer_->GetGPUVirtualAddress() + static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(offset);
	vbView_[1].SizeInBytes = bufferSize;
	vbView_[1].StrideInBytes = sizeof(VertexData);

	constBuffer_.Create(L"spriteConstBuffer", (sizeof(ConstBufferData) + 0xff) & ~0xff);
}

void SpriteData::SetTextureHandle(uint32_t textureHandle) {
	textureHandle_ = textureHandle;
	resourceDesc_ = Engine::TextureManager::GetInstance()->GetResoureDesc(textureHandle_);
}


void SpriteData::TransferVertices() {

	enum { LB, LT, RB, RT };

	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;
	if (isFlipX_) { 
		left = -left;
		right = -right;
	}

	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	// 頂点データ
	VertexData vertices[4];

	vertices[LB].pos = { left, bottom, 0.0f };  
	vertices[LT].pos = { left, top, 0.0f };     
	vertices[RB].pos = { right, bottom, 0.0f }; 
	vertices[RT].pos = { right, top, 0.0f };    

	{
		float tex_left = texBase_.x / resourceDesc_.Width;
		float tex_right = (texBase_.x + texSize_.x) / resourceDesc_.Width;
		float tex_top = texBase_.y / resourceDesc_.Height;
		float tex_bottom = (texBase_.y + texSize_.y) / resourceDesc_.Height;

		vertices[LB].uv = { tex_left, tex_bottom };  
		vertices[LT].uv = { tex_left, tex_top };     
		vertices[RB].uv = { tex_right, tex_bottom }; 
		vertices[RT].uv = { tex_right, tex_top };    
	}

	// 頂点バッファへのデータ転送
	vertexBuffer_.Copy(vertices, sizeof(vertices));
}
