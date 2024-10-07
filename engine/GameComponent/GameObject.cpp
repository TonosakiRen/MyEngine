#include "GameObject.h"
#include "ModelManager.h"
#include "DrawManager.h"
#include "GameScene.h"

ModelManager* GameObject::modelManager = ModelManager::GetInstance();
AnimationManager* GameObject::animationManager = AnimationManager::GetInstance();

void GameObject::Initialize(const std::string name)
{
	modelHandle_ = ModelManager::Load(name);
	worldTransform_.Initialize();
	material_.Initialize();
}

void GameObject::Initialize(uint32_t modelHandle)
{
	modelHandle_ = modelHandle;
	worldTransform_.Initialize();
	material_.Initialize();
}

void GameObject::Initialize()
{
	worldTransform_.Initialize(false);
	worldTransform_.Update();
	material_.Initialize();
}

void GameObject::UpdateMatrix()
{
	worldTransform_.Update(modelHandle_);
}


void GameObject::Draw(Vector4 color)
{
	material_.color_ = color;
	material_.Update();
	DrawManager::GetInstance()->DrawModel(worldTransform_,modelHandle_,0, material_);
}

void GameObject::UpdateMaterial(Vector4 color)
{
	material_.color_ = color;
	material_.Update();
}