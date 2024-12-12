#include "Stage/Mushroom.h"
#include "Draw/DrawManager.h"
#include "GameComponent/Loader.h"
#include "Easing.h"

void Mushroom::Initialize(const std::string& name, PointLight* pointLight, const Vector4& color,const float intensityT ,const Vector3& scale, const Quaternion& quaternion, const Vector3& position)
{
	LightObject::Initialize(name,pointLight, color,scale, quaternion, position);
	intensityT_ = intensityT;
}

void Mushroom::Update()
{

	if (colorT_ == 1.0f && isChangeColor_) {
		isChangeColor_ = false;
		colorT_ = 0.0f;
		color_ = initColor;
	}
	intensityT_ += intensityTSpeed_;
	intensityT_ = clamp(intensityT_, 0.1f, 1.0f);
	if (intensityT_ >= 1.0f) {
		intensityTSpeed_ *= -1.0f;
	}
	if (intensityT_ <= 0.1f) {
		intensityTSpeed_ *= -1.0f;
	}
	pointLight_->intensity = Easing::easing(intensityT_, 0.0f, 20.0f);
	colorT_ += 0.02f;
	colorT_ = clamp(colorT_, 0.0f, 1.0f);
	material_.color_ = Lerp(colorT_, material_.color_, color_);
	pointLight_->color = material_.color_;
	material_.Update();
	LightObject::Update();
}

void Mushroom::SetColor(const Vector4& color)
{
	if (isChangeColor_ == false) {
		colorT_ = 0.0f;
		color_ = color;
		isChangeColor_ = true;
	}
}