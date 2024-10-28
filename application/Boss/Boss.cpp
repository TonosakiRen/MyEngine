#include "Boss/Boss.h"

#include <numbers>
#include <algorithm>

#include "Player/Player.h"
#include "MyMath.h"
#include "Easing.h"
#include "ImGuiManager.h"
#include "Draw/DrawManager.h"
#include "Stage/Floor.h"

void Boss::Initialize(const std::string name) {
	GameObject::Initialize(name);

	collider_.Initialize(&worldTransform_,name,modelHandle_);

	stateFunctions_[kNormal] = std::bind(&Boss::Normal, this);
	stateFunctions_[kAttack] = std::bind(&Boss::Attack, this);
	stateFunctions_[kAssault] = std::bind(&Boss::Assault, this);
	stateFunctions_[kBulletAttack] = std::bind(&Boss::BulletAttack, this);

	modelWorldTransform_[static_cast<int>(Boss::Parts::Body)].Initialize();
	modelWorldTransform_[static_cast<int>(Boss::Parts::Right)].Initialize();
	modelWorldTransform_[static_cast<int>(Boss::Parts::Left)].Initialize();
	modelWorldTransform_[static_cast<int>(Boss::Parts::Body)].SetParent(&worldTransform_);
	modelWorldTransform_[static_cast<int>(Boss::Parts::Right)].SetParent(&worldTransform_);
	modelWorldTransform_[static_cast<int>(Boss::Parts::Left)].SetParent(&worldTransform_);
	modelWorldTransform_[static_cast<int>(Boss::Parts::Body)].scale_ = { 3.0f, 3.0f, 3.0f };
	worldTransform_.translation_ = { 0.0f, 1.5f, 3.0f };
	worldTransform_.quaternion_ = MakeForYAxis(Radian(180.0f));
	

	for (int i = 0; i < kBulletNum; i++) {
		bullet_[i].worldTransform_.scale_ = { 3.0f, 3.0f, 3.0f };
		bullet_[i].worldTransform_.Initialize();
	}
}

void Boss::Update() {
	enemyToPlayer_ = LoseY(player_->GetWorldTransform().translation_ - worldTransform_.translation_);
	length_ = Length(enemyToPlayer_);
	length_ = clamp(length_, 0.0f, 10000.0f);
 	
	Random();

	stateFunctions_[stats_]();

	float limit = Floor::kFloorHalfSize - modelWorldTransform_[static_cast<int>(Boss::Parts::Body)].scale_.x;
	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, -limit, limit);
	worldTransform_.translation_.z = std::clamp(worldTransform_.translation_.z, -limit, limit);
	worldTransform_.Update();
	collider_.MatrixUpdate();
	modelWorldTransform_[static_cast<int>(Boss::Parts::Body)].Update();
	modelWorldTransform_[static_cast<int>(Boss::Parts::Right)].Update();
	modelWorldTransform_[static_cast<int>(Boss::Parts::Left)].Update();

	if (isHit_ == true) {
		hp_--;
		hp_ = std::clamp(hp_, 0, 10000);
		isHit_ = false;
	}
	if (hp_ <= 0) {
		isDead_ = true;
	}
}

void Boss::Normal()
{
	enemyToPlayer_ = Normalize(LoseY(player_->GetWorldTransform().translation_ - worldTransform_.translation_));
	goalRotation_ = MakeLookRotation(enemyToPlayer_);
	worldTransform_.quaternion_ = Slerp(0.1f, worldTransform_.quaternion_, goalRotation_);

	isRotate_ = IsClose(worldTransform_.quaternion_, goalRotation_, 0.01f);


	if (isRotate_ == true) {
		isRotated_ = true;
		worldTransform_.quaternion_ = goalRotation_;
	}

	if (isRotated_ == true) {
		moveTime_++;
		if (isDecidePositive_ == false) {
			isPositive_ = Rand();
			isDecidePositive_ = true;
		}
		Vector3 move;
		if (isPositive_ == true) {
			move = { 0.1f, 0.0f, 0.0f };
		}
		else {
			move = { -0.1f, 0.0f, 0.0f };
		}
		move = move * worldTransform_.quaternion_;
		worldTransform_.translation_ = worldTransform_.translation_ + move;
	}

	if (moveTime_ > 50) {
		isRandam_ = true;
	}

}

void Boss::Attack()
{
	if (isSavePos_ == false) {
		isSavePos_ = true;
		savePos_ = worldTransform_.translation_;
		goalPos_ = { savePos_ };
		Vector3 goalMove = { 0.0f, 0.0f, 8.0f };
		goalMove = goalMove * worldTransform_.quaternion_;
		goalPos_ = goalMove + savePos_;
		attackCooltime_ = 60;
		attackT_ = 0.0f;
	}

	worldTransform_.translation_ =
		Easing::easing(attackT_, savePos_, goalPos_, attackSpeed_, Easing::easeInQuint);
	modelWorldTransform_[static_cast<int>(Boss::Parts::Right)].translation_ = Easing::Bezier(
		{ 0.0f, 0.0f, 0.0f }, { 12.0f, 0.0f, 3.0f }, { -5.0f, 0.0f, 12.0f }, attackT_, attackSpeed_,
		Easing::easeInQuint, false);

	modelWorldTransform_[static_cast<int>(Boss::Parts::Right)].scale_ = Easing::easing(
		attackT_, { 1.0f, 1.0f, 1.0f }, { 2.0f, 2.0f, 2.0f }, attackSpeed_, Easing::easeInQuint);


	if (attackT_ >= 1.0f) {
		attackCooltime_--;
	}

	if (attackCooltime_ < 0.0f) {
		modelWorldTransform_[static_cast<int>(Boss::Parts::Right)].translation_ = {
			0.0f, 0.0f, 0.0f };
		isRandam_ = true;
	}
}

void Boss::Assault()
{
	//回転
	if (assaultTime_ >= 0) {
		degreeSpeed_ += Radian(2.0f);
		degreeSpeed_ = std::clamp(degreeSpeed_, 0.0f, 5.8f);
		degree_ += degreeSpeed_;
		if (degree_ >= 360.0f) {
			degree_ = degree_ - 360.0f;
		}
	}
	else {
		if (isEndAssult_ == false) {
			degreeSpeed_ -= Radian(2.0f);
			degreeSpeed_ = std::clamp(degreeSpeed_, 0.0f, 5.8f);
			degree_ += degreeSpeed_;
		}
		if (degree_ >= 360.0f) {
			isEndAssult_ = true;
			degree_ = 0.0f;
			isRandam_ = true;
		}
	}

	if (isAssault_ == false) {
		assaultTime_ = 120;
		enemyToPlayer_ = Normalize(LoseY(player_->GetWorldTransform().translation_ - worldTransform_.translation_));
		goalRotation_ = MakeLookRotation(enemyToPlayer_);
		worldTransform_.quaternion_ = Slerp(0.01f, worldTransform_.quaternion_, goalRotation_);
	}
	//移動
	if (degreeSpeed_ >= 5.8f && isAssault_ == false) {
		isAssault_ = true;
		assaultDirection_ = enemyToPlayer_;
	}
	if (isAssault_ == true && isEndAssult_ == false) {
		assaultTime_--;
		Vector3 move = assaultDirection_ * 0.2f;
		worldTransform_.translation_ = worldTransform_.translation_ + move;
	}
	worldTransform_.quaternion_ = worldTransform_.quaternion_ * MakeForXAxis(degreeSpeed_);
}

void Boss::BulletAttack()
{
	degreeSpeed_ += Radian(4.0f);
	degree_ = degreeSpeed_;
	if (degree_ >= 360.0f) {
		degree_ = degree_ - 360.0f;
	}
	bulletCooltime_--;
	if (bulletCooltime_ <= 0 && bulletShotAllNum_ < 6) {
		bulletShotNum_ = 0;
		for (int i = 0; i < kBulletNum; i++) {
			if (bullet_[i].isActive_ == false) {
				bullet_[i].lifeSpan_ = 0;
				bullet_[i].isActive_ = true;
				Quaternion rotateMatrix = MakeForYAxis(Radian(90.0f * bulletShotNum_) + Radian(2.0f * bulletShotAllNum_));
				bullet_[i].direction_ = Vector3{ 1.0f, 0.0f, 0.0f } *rotateMatrix;
				bullet_[i].worldTransform_.translation_ = worldTransform_.translation_;
				bullet_[i].worldTransform_.quaternion_ = MakeLookRotation(bullet_[i].direction_);
				bulletShotNum_++;
				bulletLastNum_++;
				if (bulletShotNum_ == 4) {
					bulletCooltime_ = 20;
					bulletShotAllNum_++;
					break;
				}
			}
		}
	}

	for (int i = 0; i < kBulletNum; i++) {
		if (bullet_[i].isActive_ == true) {
			bullet_[i].lifeSpan_++;
			bullet_[i].worldTransform_.translation_ += bullet_[i].direction_ * bulletSpeed_;
			bullet_[i].worldTransform_.Update();
			if (bullet_[i].lifeSpan_ >= 240) {
				bullet_[i].isActive_ = false;
			}
		}
	}
	if (bullet_[bulletLastNum_ - 1].isActive_ == false && bulletShotAllNum_ == 6) {
		isRandam_ = true;
	}
	worldTransform_.quaternion_ = worldTransform_.quaternion_ * MakeForYAxis(degreeSpeed_);
}

void Boss::Random()
{
	if (isRandam_ == true) {
		isRandam_ = false;
		preStats_ = stats_;
		randam_ = Rand(0, 100);
		if (length_ <= 25.0f) {
			if (randam_ <= 50) {
				stats_ = kNormal;
			}
			else if (randam_ <= 90) {
				stats_ = kAttack;
			}
			else if (randam_ <= 95) {
				stats_ = kBulletAttack;
			}
			else if (randam_ <= 100) {
				stats_ = kAssault;
			}

		}
		else if (length_ >= 35.0f) {
			if (randam_ <= 50) {
				stats_ = kNormal;
			}
			else if (randam_ <= 80) {
				stats_ = kBulletAttack;
			}
			else if (randam_ <= 100) {
				stats_ = kAssault;
			}
		}
		else {
			if (randam_ <= 50) {
				stats_ = kNormal;
			}
			else if (randam_ <= 75) {
				stats_ = kBulletAttack;
			}
			else if (randam_ <= 100) {
				stats_ = kAssault;
			}
		}

		if (preStats_ != kNormal) {
			stats_ = kNormal;
		}

		//初期化
		isRotated_ = false;
		isRotate_ = false;
		isDecidePositive_ = false;
		moveTime_ = 0;

		isSavePos_ = false;

		degree_ = 0.0f;
		degreeSpeed_ = 0.0f;
		isAssault_ = false;
		assaultTime_ = 0;
		isEndAssult_ = false;

		for (int i = 0; i < kBulletNum; i++) {

			bullet_[i].isActive_ = false;
			bullet_[i].lifeSpan_ = 0;
			bullet_[i].direction_ = { 0.0f, 0.0f, 0.0f };
			bullet_[i].worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
		}

		bulletCooltime_ = 0;
		bulletShotNum_ = 0;
		bulletShotAllNum_ = 0;
		bulletLastNum_ = 0;
	}
}

void Boss::Draw() {
	for (int i = 0; i < kBulletNum; i++) {
		if (bullet_[i].isActive_ == true) {
			DrawManager::GetInstance()->DrawMeshletModel(bullet_[i].worldTransform_);
		}
	}
	for (int i = 0; i < int(Parts::PartsNum); i++) {
		DrawManager::GetInstance()->DrawMeshletModel(modelWorldTransform_[i]);
	}
}