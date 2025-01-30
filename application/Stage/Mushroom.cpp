/**
 * @file Mushroom.cpp
 * @brief キノコ
 */

#include "Stage/Mushroom.h"
#include "Draw/DrawManager.h"
#include "GameComponent/Loader.h"
#include "Easing.h"

void Mushroom::Initialize(const std::string& name,WorldTransform* parentWorldTransform, PointLight* pointLight, const Vector4& color,const float intensityT ,const Vector3& scale, const Quaternion& quaternion, const Vector3& position)
{
	LightObject::Initialize(name,pointLight, color,scale, quaternion, position);
	worldTransform_.SetParent(parentWorldTransform, false);
	worldTransform_.translation_ = position;
	worldTransform_.quaternion_ = quaternion;
	//mushroomParticle_.Initialize(worldTransform_);
	intensityT_ = intensityT;
}

void Mushroom::Update()
{
	const float maxIntensity_ = 20.0f;
	const float colorAddValue = 0.02f;


	//発光を増減させる
	intensityT_ += intensityTSpeed_;
	intensityT_ = Clamp(intensityT_, 0.1f, 1.0f);
	if (intensityT_ >= 1.0f) {
		intensityTSpeed_ *= -1.0f;
	}
	if (intensityT_ <= 0.1f) {
		intensityTSpeed_ *= -1.0f;
	}
	pointLight_->intensity = Easing::easing(intensityT_, 0.0f, maxIntensity_);

	//カラーが変更されていてtが1.0fになったら初期化
	if (colorT_ == 1.0f && isChangeColor_) {
		isChangeColor_ = false;
		colorT_ = 0.0f;
		color_ = initColor;
	}
	colorT_ += colorAddValue;
	colorT_ = Clamp(colorT_, 0.0f, 1.0f);
	material_.color_ = Lerp(colorT_, material_.color_, color_);
	pointLight_->color = material_.color_;

	//ParticleUpdate
	//mushroomParticle_.SetColor(color_);
	//mushroomParticle_.Update();

	material_.Update();
	LightObject::Update();
}

void Mushroom::PushDataParticle(std::unique_ptr<ParticleData>& particleData)
{
	particleData;
	//mushroomParticle_.PushDataParticle(particleData);
}

void Mushroom::SetColor(const Vector4& color)
{
	if (isChangeColor_ == false) {
		colorT_ = 0.0f;
		color_ = color;
		isChangeColor_ = true;
	}
}