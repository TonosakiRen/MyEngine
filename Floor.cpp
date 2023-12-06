#include "Floor.h"
#include "ImGuiManager.h"
void Floor::Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight)
{
	GameObject::Initialize(name, viewProjection, directionalLight);
	material_.enableLighting_ = false;
	collider_.Initialize(&worldTransform_,name, viewProjection, directionalLight,{20.0f, 1.0f, 1.0f});
}

void Floor::Update()
{
	collider_.AdjustmentScale();
	material_.translation_.y += -0.001f;
	ImGui::DragFloat3("a", &worldTransform_.translation_.x, 0.01f);
	UpdateMatrix();
}

void Floor::Draw() {
	collider_.Draw();
	GameObject::Draw();
}