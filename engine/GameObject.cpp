#include "GameObject.h"
#include "ModelManager.h"
#include "Model.h"
#include "ShadowMap.h"
#include "SpotLightShadowMap.h"

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

void GameObject::UpdateMatrix()
{
	worldTransform_.Update();
}

void GameObject::Draw(uint32_t textureHandle, Vector4 color)
{
	if (ShadowMap::isDrawShadowMap) {
		ShadowMap::Draw(modelHandle_, worldTransform_);
	}
	else if (SpotLightShadowMap::isDrawSpotLightShadowMap) {
		SpotLightShadowMap::Draw(modelHandle_, worldTransform_);
	}
	else {
		material_.color_ = color;
		material_.Update();
		Model::Draw(modelHandle_, worldTransform_, material_, textureHandle);
	}
}

void GameObject::Draw(Vector4 color)
{
	if (ShadowMap::isDrawShadowMap) {
		ShadowMap::Draw(modelHandle_, worldTransform_);
	}
	else if (SpotLightShadowMap::isDrawSpotLightShadowMap) {
		SpotLightShadowMap::Draw(modelHandle_, worldTransform_);
	}
	else {
		material_.color_ = color;
		material_.Update();
		Model::Draw(modelHandle_, worldTransform_, material_);
	}
}

void GameObject::Draw(const WorldTransform& worldTransform, Vector4 color)
{
	if (ShadowMap::isDrawShadowMap) {
		ShadowMap::Draw(modelHandle_, worldTransform_);
	}
	else if (SpotLightShadowMap::isDrawSpotLightShadowMap) {
		SpotLightShadowMap::Draw(modelHandle_, worldTransform_);
	}
	else {
		material_.color_ = color;
		material_.Update();
		Model::Draw(modelHandle_, worldTransform, material_);
	}
}

void GameObject::Draw(const WorldTransform& worldTransform, uint32_t textureHandle, Vector4 color)
{
	if (ShadowMap::isDrawShadowMap) {
		ShadowMap::Draw(modelHandle_, worldTransform_);
	}
	else if (SpotLightShadowMap::isDrawSpotLightShadowMap) {
		SpotLightShadowMap::Draw(modelHandle_, worldTransform_);
	}
	else {
		material_.color_ = color;
		material_.Update();
		Model::Draw(modelHandle_, worldTransform, material_, textureHandle);
	}
}

void GameObject::UpdateMaterial(Vector4 color)
{
	material_.color_ = color;
	material_.Update();
}