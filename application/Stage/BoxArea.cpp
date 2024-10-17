#include "Stage/BoxArea.h"
#include "Texture/TextureManager.h"
#include "ImGuiManager.h"
#include "Draw/DrawManager.h"

void BoxArea::Initialize(const std::string name)
{
	GameObject::Initialize(name);
	material_.enableLighting_ = true;
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
	DrawManager::GetInstance()->DrawModel(worldTransform_,modelHandle_, textureHandle_, material_);
}

