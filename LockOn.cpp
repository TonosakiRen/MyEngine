#include "LockOn.h"
#include "ViewProjection.h"
#include "TextureManager.h"
#include "LockOn.h"

void LockOn::Initialize()
{
	input_ = Input::GetInstance();
	uint32_t textureHandle = TextureManager::Load("reticle.png");
	reticle_.reset(Sprite::Create(textureHandle, { 0.0f,0.0f }));
}

void LockOn::Update(const std::list<std::unique_ptr<Boss>>& enemies, const ViewProjection& viewProjection)
{
	if (input_->TriggerKey(DIK_F) || input_->TriggerButton(XINPUT_GAMEPAD_Y)) {
		if (isLookOn_ == false ) {
			isLookOn_ = true;
			SearchTarget(enemies, viewProjection);
		}
		else {
			isLookOn_ = false;
			target_ = nullptr;
		}
	}
	
	if (input_->TriggerKey(DIK_E) || input_->TriggerButton(XINPUT_GAMEPAD_B) ) {
		if (isLookOn_ == true) {
			SearchTarget(enemies, viewProjection);
		}
	}



	if (isLookOn_ == true) {
		if (target_) {
			if (OutOfRangeTarget(viewProjection)) {
				target_ = nullptr;
				isLookOn_ = false;
			}
			if (target_ && target_->GetIsDead()) {
				target_ = nullptr;
				isLookOn_ = false;
			}
		}
		
	}
	

	if (target_){
		Vector3 positionWorld = MakeTranslation(target_->GetWorldTransform()->matWorld_);
		Vector2 positionScreen = viewProjection.MakeScreenVector(positionWorld);
		reticle_->position_ = positionScreen;
	}

}

void LockOn::Draw()
{
	if (target_) {
		reticle_->Draw();
	}
}

bool LockOn::OutOfRangeTarget(const ViewProjection& viewProjection)
{
	Vector3 positionWorld = MakeTranslation(target_->GetWorldTransform()->matWorld_);
	Vector3 positionView = Transform(positionWorld, viewProjection.GetMatView());
	if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {
		/*float arcTangent = std::atan2(
			std::sqrt(positionView.x * positionView.x + positionView.y * positionView.y),
			positionView.z);*/

		Vector3 s = { std::sqrt(positionView.x * positionView.x + positionView.y * positionView.y) ,0.0f,positionView.z };

		float arcTangent = Angle({ 0.0f,0.0f,1.0f }, s);



		if (Cross({ 0.0f,1.0f }, Vector2{ s.x,s.z }) < 0.0f) {
			arcTangent = arcTangent;
		}
		else {
			arcTangent = -arcTangent;
		}

		if (std::fabsf(arcTangent) <= angleTange_) {
			return false;
		}
	}
	return true;
}

void LockOn::SearchTarget(const std::list<std::unique_ptr<Boss>>& enemies, const ViewProjection& viewProjection) {
	std::list<std::pair<float, const Boss*>> targets;

	for (const std::unique_ptr<Boss>& boss : enemies) {
		if (!boss->GetIsDead()) {
			Vector3 positionWorld = MakeTranslation(boss->GetWorldTransform()->matWorld_);
			Vector3 positionView = Transform(positionWorld, viewProjection.GetMatView());

			if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {
				Vector3 s = { std::sqrt(positionView.x * positionView.x + positionView.y * positionView.y) ,0.0f,positionView.z };

				float arcTangent = Angle({ 0.0f,0.0f,1.0f }, s);

				if (std::fabsf(arcTangent) <= angleTange_) {
					targets.emplace_back(std::make_pair(Length(positionView), boss.get()));
				}
			}
		}
	}

	if (!targets.empty()) {
		targets.sort([](auto& pair1, auto& pair2) {return pair1.first < pair2.first; });
		if (input_->TriggerKey(DIK_E)) {
			for (auto& target : targets) {
				if (target.second == target_) {
					continue;
				}
				else {
					target_ = target.second;
					break;
				}
			}
		}
		else {
			target_ = targets.front().second;
		}
		
	}
}
