/**
 * @file Skybox.cpp
 * @brief skybox
 */
#include "Skybox.h"
#include "Draw/DrawManager.h"

void Skybox::Initialize(const std::string name)
{
	GameObject::Initialize(name);
	material_.enableLighting_ = false;
	material_.Update();
	const float scale = 200.0f;
	worldTransform_.scale_ = {scale,scale,scale };
}

void Skybox::Update()
{
	worldTransform_.Update();
}

void Skybox::Draw()
{
	Engine::DrawManager::GetInstance()->DrawSky(worldTransform_);
}
