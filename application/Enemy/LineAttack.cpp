/**
 * @file LineAttack.cpp
 * @brief ライン上の攻撃
 */
#include "Enemy/LineAttack.h"
#include "Enemy/EnemyBulletManager.h"
#include "Player/Player.h"
#include "Model/ModelManager.h"
#include "Stage/Floor.h"
#include "Light/LightManager.h"
#include "Draw/DrawManager.h"


void LineAttack::Initialize()
{
	GameObject::Initialize(0);
	lightManager_ = LightManager::GetInstance();
	
}

void LineAttack::Update()
{
	const float move = 0.2f;
	const Vector4 color = { 0.7f,0.7f,1.0f,1.0f };
	const float attackStartFrame = 40;
	const float attackEndFrame = 60;
	const float growHeight = -3.5f;
	const float scaleSpeed = 0.5f;

	if (isEmit_) {
		if (lightLength_ <= Floor::kFloorSize * 2.0f) {
			lightLength_ += move;
		}
		lightManager_->areaLights_->lights_[0].color = color;
		lightManager_->areaLights_->lights_[0].isActive = true;
		Segment segment;
		if (isBox_ == false) {
			segment.origin = { -Floor::kFloorHalfSize,1.0f, 0.0f };
			lastLength_ = -Floor::kFloorHalfSize;
		}
		else {
			segment.origin = { lastLength_,1.0f, 0.0f };
		}
		segment.diff = { lightLength_ ,0.0f,0.0f };
		lightManager_->areaLights_->lights_[0].segment = segment;
		if (lightLength_ - emitLength_ >= 0.0f  && emitLength_ <= Floor::kFloorSize) {
			isBox_ = true;
			emitLength_+= kEmitLength_;
			std::unique_ptr<Box> box;
			box = std::make_unique<Box>();
			box->worldTransform_.Initialize();
			box->worldTransform_.translation_ = { -Floor::kFloorHalfSize + lightLength_,growHeight, 0.0f};
			box->direction_ = Normalize(Vector3{0.0f,Rand(0.8f,1.0f),Rand(-0.2f,0.2f) });
			box->worldTransform_.quaternion_ = MakeFromTwoVector({ 0.0f,1.0f,0.0f }, box->direction_);
			box->worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
			box->attackStartFrame_ = 0;
			box->endFrame_ = 0;
			box->isActive_ = true;
			boxs_.push_back(std::move(box));
		}
	}

	for (auto& box : boxs_) {
		box->attackStartFrame_++;
		if (box->attackStartFrame_ >= attackStartFrame && box->worldTransform_.scale_.y <= scale_ && box->endFrame_ == 0) {
			box->worldTransform_.scale_.y += scaleSpeed;
			box->worldTransform_.translation_ += box->direction_ * Rand(0.2f,0.6f);
		}
		else if (box->attackStartFrame_ >= attackStartFrame) {
			box->endFrame_++;
			if (box->endFrame_ >= attackEndFrame) {
				box->worldTransform_.scale_.y -= scaleSpeed;
				box->worldTransform_.translation_ += box->direction_ * Rand(-0.2f, -0.6f);
				if (box->worldTransform_.scale_.y <= 0.1f) {
					box->isActive_ = false;
				}
			}
		}
		box->worldTransform_.Update();
	}

	boxs_.erase(std::remove_if(boxs_.begin(),boxs_.end(),
			[&](const std::unique_ptr<Box>& box) {
				if (box->isActive_) {
					return false;
				}
				else {
					lastLength_ = box->worldTransform_.translation_.x;
					return true;
				}
			}
		),
		boxs_.end()
	);
	
	if (isEmit_ == true && isBox_ == true && boxs_.empty()) {
		isEmit_ = false;
		isBox_ = false;
		lightLength_ = 0.0f;
		emitLength_ = kEmitLength_;
	}

	worldTransform_.Update();
}

void LineAttack::Draw()
{
	DrawManager* drawManager_ = DrawManager::GetInstance();
	for (auto& box : boxs_) {
		drawManager_->DrawModel(box->worldTransform_);
	}
}
