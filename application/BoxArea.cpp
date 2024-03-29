#include "BoxArea.h"
#include "TextureManager.h"
#include "ImGuiManager.h"

void BoxArea::Initialize(const std::string name)
{
	GameObject::Initialize(name);
	material_.enableLighting_ = false;
	material_.Update();
	worldTransform_.scale_ = { size_, size_, size_ };
	worldTransform_.translation_.y = size_ / 2.0f;
	textureHandle_ = TextureManager::Load("tile.png");
}

void BoxArea::Update()
{
	worldTransform_.Update();
}

void BoxArea::Draw()
{
	GameObject::Draw(worldTransform_, textureHandle_);
}

