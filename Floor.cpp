#include "Floor.h"
#include "ImGuiManager.h"
Floor* Floor::Create(const std::string name)
{
	Floor* object3d = new Floor();
	assert(object3d);

	object3d->Initialize(name);
	object3d->material_.enableLighting_ = false;

	return object3d;
}

void Floor::Update()
{
	material_.translation_.y += -0.001f;
}
