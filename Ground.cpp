#include "Ground.h"
#include "ImGuiManager.h"

void Ground::Initialize( ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector3 initialPos)
{
	GameObject::Initialize("box6x6", viewProjection, directionalLight);
	worldTransform_.translation_ = { initialPos };
	worldTransform_.UpdateMatrix();
	collider_.Initialize(&worldTransform_,"ground",*viewProjection, *directionalLight,{6.0f,6.0f,6.0f});

}

void Ground::Update()
{
	if (isMove_) {
		worldTransform_.translation_.z += speed_;
	}
	if (worldTransform_.translation_.z >= 30.0f || worldTransform_.translation_.z <= 15.0f) {
		speed_ *= -1.0f;
	}
	worldTransform_.UpdateMatrix();
	collider_.MatrixUpdate();
}
void Ground::Draw() {
	GameObject::Draw({ 1.0f,1.0f,1.0f });
}