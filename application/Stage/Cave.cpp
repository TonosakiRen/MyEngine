#include "Stage/Cave.h"
#include "Texture/TextureManager.h"
#include "ImGuiManager.h"
#include "Draw/DrawManager.h"

void Cave::Initialize()
{
	GameObject::Initialize("cave.obj");
	material_.Update();
	worldTransform_.translation_.y = 9.04f;
	worldTransform_.translation_.z = 102.6f;
	mushRooms_ = std::make_unique<Mushrooms>();
	mushRooms_->Initialize("mushroom.obj", worldTransform_);

}

void Cave::Update()
{
	mushRooms_->Update();
	worldTransform_.Update();
}

void Cave::Draw()
{
	mushRooms_->Draw();
	DrawManager::GetInstance()->DrawMeshletModel(worldTransform_,modelHandle_);
}


