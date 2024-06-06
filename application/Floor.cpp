#include "Floor.h"
#include "TextureManager.h"
#include "ImGuiManager.h"
#include "DrawManager.h"

void Floor::Initialize(const std::string name)
{
	GameObject::Initialize(name);
	material_.enableLighting_ = true;
	material_.Update();
	worldTransform_.scale_ = { size_, size_, size_ };
	worldTransform_.Update();
	textureHandle_ = TextureManager::Load("floorTile.png");
}

void Floor::Update()
{
	worldTransform_.Update();
}

void Floor::Draw()
{
	DrawManager::GetInstance()->DrawFloor(worldTransform_,modelHandle_);
}

