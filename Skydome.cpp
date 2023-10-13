#include "Skydome.h"

Skydome* Skydome::Create(const std::string name)
{
	Skydome* object3d = new Skydome();
	assert(object3d);

	object3d->Initialize(name);
	object3d->material_.enableLighting_ = false;
	object3d->worldTransform_.scale_ = object3d->worldTransform_.scale_ * 50.0f;

	return object3d;
}

void Skydome::Update()
{
	worldTransform_.rotation_.y -= 0.002f;
	worldTransform_.UpdateMatrix();
}
