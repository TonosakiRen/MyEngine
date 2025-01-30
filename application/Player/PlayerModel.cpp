/**
 * @file PlayerModel.cpp
 * @brief PlayerのModel
 */
#include "Player/PlayerModel.h"
#include "ImGuiManager.h"
#include "Player/PlayerBulletManager.h"
#include "Texture/TextureManager.h"
#include "Audio.h"
#include "Render/Wire.h"
#include "Draw/DrawManager.h"
#include "Stage/Floor.h"

void PlayerModel::Initialize(WorldTransform& worldTransform)
{
	animationManager_ = Engine::AnimationManager::GetInstance();
	modelManager_ = Engine::ModelManager::GetInstance();
	for (int i = 0; i < kMovingNum; i++) {
		animationTime_[i] = 0.0f;
	}
	animation_[kStand] = animationManager_->Load("stand.gltf", "Armature|mixamo.com|Layer0");
	animation_[kWalk] = animationManager_->Load("walk.gltf", "Armature|mixamo.com|Layer0");
	uint32_t modelHandle = modelManager_->Load("walk.gltf");
	modelHandle_ = modelHandle;
	skeleton_.Create(modelManager_->GetRootNode(modelHandle));
	skinCluster_.Create(skeleton_, modelHandle);
	
	playerWorldTransform_ = &worldTransform;
}

void PlayerModel::Update()
{
	AnimationUpdate();
	skeleton_.Update();
	skinCluster_.Update();
}


void PlayerModel::Draw() {
	
	Engine::DrawManager::GetInstance()->DrawManager::DrawMeshletModel(*playerWorldTransform_,modelHandle_,skinCluster_);

}


void PlayerModel::AnimationUpdate()
{

	//Animation遷移
	if (currentMoving_ != nextMoving_) {
		animationTime_[nextMoving_] += 1.0f / 60.0f;
		animationTime_[nextMoving_] = std::fmod(animationTime_[nextMoving_], animation_[nextMoving_].duration);
		transitionAnimationT_ += transitionSpeed_;
		Engine::AnimationManager::LerpSkeleton(transitionAnimationT_, skeleton_, animation_[currentMoving_], animationTime_[currentMoving_], animation_[nextMoving_], animationTime_[nextMoving_]);
		if (transitionAnimationT_ >= 1.0f) {
			animationTime_[currentMoving_] = 0.0f;
			transitionAnimationT_ = 0.0f;
			currentMoving_ = nextMoving_;
		}
	}
	else {
		animationTime_[currentMoving_] += 1.0f / 60.0f;
		animationTime_[currentMoving_] = std::fmod(animationTime_[currentMoving_], animation_[currentMoving_].duration);
		Engine::AnimationManager::ApplyAnimation(skeleton_, animation_[currentMoving_], animationTime_[currentMoving_]);
	}
}
