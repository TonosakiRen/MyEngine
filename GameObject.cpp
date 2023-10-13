#include "GameObject.h"

GameObject* GameObject::Create(const std::string name) {
	GameObject* object3d = new GameObject();
	assert(object3d);

	object3d->Initialize(name);

	return object3d;
}


void GameObject::Initialize(const std::string name)
{
	model_.Initialize(name);
	worldTransform_.Initialize();
	material_.Initialize();
}

void GameObject::Update()
{
	worldTransform_.UpdateMatrix();
}

void GameObject::Draw(ViewProjection& viewProjection, const DirectionalLight& directionalLight, Vector4 color)
{
	material_.color_ = color;
	material_.UpdateMaterial();
	model_.Draw(worldTransform_, viewProjection, directionalLight, material_);
}
