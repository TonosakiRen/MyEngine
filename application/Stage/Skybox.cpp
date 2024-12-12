#include "Skybox.h"
#include "Draw/DrawManager.h"

void Skybox::Initialize(const std::string name)
{
	GameObject::Initialize(name);
	material_.enableLighting_ = false;
	material_.Update();
	worldTransform_.scale_ = {200.0f,200.0f,200.0f };
}

void Skybox::Update()
{
	worldTransform_.Update();
}

void Skybox::Draw()
{
	DrawManager::GetInstance()->DrawSky(worldTransform_);
}
