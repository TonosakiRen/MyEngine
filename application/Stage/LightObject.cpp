#include "Stage/LightObject.h"
#include "Draw/DrawManager.h"
#include "GameComponent/Loader.h"

void LightObject::Initialize(const std::string& name,PointLight* pointLight,const Vector4& color,const Vector3& scale, const Quaternion& quaternion,const Vector3& position)
{
	GameObject::Initialize(name);
	worldTransform_.scale_ = scale;
	worldTransform_.translation_ = position;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.Update();
	collider_.Initialize(&worldTransform_, name, modelHandle_);
	material_.color_ = color;
	isActive_ = true;
	pointLight_ = pointLight;
}

void LightObject::Update()
{
	worldTransform_.Update();
	material_.Update();
}

void LightObject::SetColor(const Vector4& color)
{
	material_.color_ = color;
	pointLight_->color = material_.color_;
}
