#include "GoalBox.h"
#include "ImGuiManager.h"

void GoalBox::Initialize(ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector3 initialPos)
{
	GameObject::Initialize("box6x6", viewProjection, directionalLight);
	worldTransform_.translation_ = { initialPos };
	worldTransform_.UpdateMatrix();
	collider_.Initialize(&worldTransform_, "ground", viewProjection, directionalLight, { 1.0f,1.0f,1.0f });

}

void GoalBox::Update()
{
	
	worldTransform_.UpdateMatrix();
	collider_.MatrixUpdate();
}
void GoalBox::Draw() {
	collider_.Draw({ 1.0f,1.0f,0.0f,1.0f });
}