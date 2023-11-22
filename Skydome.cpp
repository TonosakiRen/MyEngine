#include "Skydome.h"

void Skydome::Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight)
{
	GameObject::Initialize(name, viewProjection, directionalLight);
	material_.enableLighting_ = false;
	material_.Update();
	worldTransform_.scale_ = worldTransform_.scale_ * 50.0f;
}

void Skydome::Update()
{
	worldTransform_.Update();
}