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
#include "Scene/GameScene.h"
#include "Stage/Floor.h"

void PlayerModel::Initialize(WorldTransform& worldTransform)
{
	animationManager_ = AnimationManager::GetInstance();
	modelManager_ = ModelManager::GetInstance();
	for (int i = 0; i < kMovingNum; i++) {
		animationTime_[i] = 0.0f;
	}
	animation_[kStand] = animationManager_->Load("stand.gltf", "Armature|mixamo.com|Layer0");
	animation_[kWalk] = animationManager_->Load("walk.gltf", "Armature|mixamo.com|Layer0");
	uint32_t modelHandle = modelManager_->Load("walk.gltf");
	modelHandle_ = modelHandle;
	skeleton_.Create(modelManager_->GetRootNode(modelHandle));
	skinCluster_.Create(skeleton_, modelHandle);
	
	//部位のWordTrasnform初期化
	for (int i = 0;  JointWorldTransform & joint : jointWordTransforms_) {
		joint.worldTransform.Initialize();
		joint.worldTransform.SetParent(&worldTransform);
		partsWordTransform_[i].worldTransform.Initialize();
		partsWordTransform_[i].worldTransform.SetParent(&joint.worldTransform);
		i++;
	}

	jointWordTransforms_[HeadTop].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:HeadTop_End").skeletonSpaceMatrix;
	partsWordTransform_[HeadTop].worldTransform.scale_ = { 10.0f,1.0f,10.0f };
	jointWordTransforms_[Head].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:Head").skeletonSpaceMatrix;
	partsWordTransform_[Head].worldTransform.scale_ = { 15.0f,1.0f,2.0f };
	jointWordTransforms_[SpineTop].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:Spine2").skeletonSpaceMatrix;
	partsWordTransform_[SpineTop].worldTransform.scale_ = { 5.0f,1.0f,5.0f };


	jointWordTransforms_[RightShoulder].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:RightShoulder").skeletonSpaceMatrix;

	jointWordTransforms_[RightArm].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:RightArm").skeletonSpaceMatrix;
	partsWordTransform_[RightArm].worldTransform.scale_ = { 5.0f,1.0f,5.0f };
	jointWordTransforms_[RightForArm].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:RightForeArm").skeletonSpaceMatrix;
	partsWordTransform_[RightForArm].worldTransform.scale_ = { 5.0f,1.0f,5.0f };
	jointWordTransforms_[RightHand].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:RightHand").skeletonSpaceMatrix;
	partsWordTransform_[RightHand].worldTransform.scale_ = { 10.0f,1.0f,10.0f };
	
	jointWordTransforms_[LeftShoulder].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:LeftShoulder").skeletonSpaceMatrix;

	jointWordTransforms_[LeftArm].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:LeftArm").skeletonSpaceMatrix;
	jointWordTransforms_[LeftForArm].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:LeftForeArm").skeletonSpaceMatrix;
	jointWordTransforms_[LeftHand].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:LeftHand").skeletonSpaceMatrix;
	partsWordTransform_[LeftArm].worldTransform.scale_ = partsWordTransform_[RightArm].worldTransform.scale_;
	partsWordTransform_[LeftForArm].worldTransform.scale_ = partsWordTransform_[RightForArm].worldTransform.scale_;
	partsWordTransform_[LeftHand].worldTransform.scale_ = partsWordTransform_[RightHand].worldTransform.scale_;

	jointWordTransforms_[SpineBottom].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:Spine").skeletonSpaceMatrix;
	partsWordTransform_[SpineBottom].worldTransform.scale_ = { 10.0f,1.0f,5.0f };

	jointWordTransforms_[RightUpLeg].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:RightUpLeg").skeletonSpaceMatrix;
	partsWordTransform_[RightUpLeg].worldTransform.scale_ = { 3.0f,1.0f,10.0f };
	jointWordTransforms_[RightLeg].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:RightLeg").skeletonSpaceMatrix;
	partsWordTransform_[RightLeg].worldTransform.scale_ = { 5.0f,1.0f,15.0f };
	jointWordTransforms_[RightFoot].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:RightFoot").skeletonSpaceMatrix;
	partsWordTransform_[RightFoot].worldTransform.scale_ = { 5.0f,1.0f,20.0f };

	jointWordTransforms_[LeftUpLeg].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:LeftUpLeg").skeletonSpaceMatrix;
	jointWordTransforms_[LeftLeg].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:LeftLeg").skeletonSpaceMatrix;
	jointWordTransforms_[LeftFoot].skeletonSpaceMatrix = &skeleton_.GetJoint("mixamorig:LeftFoot").skeletonSpaceMatrix;

	partsWordTransform_[LeftUpLeg].worldTransform.scale_ = partsWordTransform_[RightUpLeg].worldTransform.scale_;
	partsWordTransform_[LeftLeg].worldTransform.scale_ = partsWordTransform_[RightLeg].worldTransform.scale_;
	partsWordTransform_[LeftFoot].worldTransform.scale_ = partsWordTransform_[RightFoot].worldTransform.scale_;

	playerWorldTransform_ = &worldTransform;
}

void PlayerModel::Update()
{

	partsWordTransform_[LeftArm].worldTransform.scale_ = partsWordTransform_[RightArm].worldTransform.scale_;
	partsWordTransform_[LeftForArm].worldTransform.scale_ = partsWordTransform_[RightForArm].worldTransform.scale_;
	partsWordTransform_[LeftHand].worldTransform.scale_ = partsWordTransform_[RightHand].worldTransform.scale_;

	partsWordTransform_[LeftUpLeg].worldTransform.scale_ = partsWordTransform_[RightUpLeg].worldTransform.scale_;
	partsWordTransform_[LeftLeg].worldTransform.scale_ = partsWordTransform_[RightLeg].worldTransform.scale_;
	partsWordTransform_[LeftFoot].worldTransform.scale_ = partsWordTransform_[RightFoot].worldTransform.scale_;


	AnimationUpdate();
	skeleton_.Update();
	skinCluster_.Update();
}


void PlayerModel::Draw() {
	
	for (JointWorldTransform& joint : jointWordTransforms_) {
		joint.worldTransform.Update(*joint.skeletonSpaceMatrix);
	}


	/*partsWordTransform_[HeadTop].worldTransform.quaternion_ *= MakeForYAxis(Radian(2.0f));
	partsWordTransform_[SpineBottom].worldTransform.quaternion_ *= MakeForYAxis(Radian(2.0f));*/
	for (JointWorldTransform& joint : partsWordTransform_) {
		joint.worldTransform.Update();
	}

	/*DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[HeadTop].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));
	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[Head].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));
	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[SpineTop].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));

	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[RightArm].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));
	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[RightForArm].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));
	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[RightHand].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));
	
	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[LeftArm].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));
	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[LeftForArm].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));
	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[LeftHand].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));

	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[SpineBottom].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));

	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[RightUpLeg].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));
	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[RightLeg].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));
	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[RightFoot].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));

	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[LeftUpLeg].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));
	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[LeftLeg].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));
	DrawManager::GetInstance()->DrawManager::DrawModel(partsWordTransform_[LeftFoot].worldTransform, ModelManager::GetInstance()->Load("box1x1.obj"));*/

	DrawManager::GetInstance()->DrawManager::DrawMeshletModel(*playerWorldTransform_,modelHandle_,skinCluster_);

}


void PlayerModel::AnimationUpdate()
{

	//Animation遷移
	if (currentMoving_ != nextMoving_) {
		animationTime_[nextMoving_] += 1.0f / 60.0f;
		animationTime_[nextMoving_] = std::fmod(animationTime_[nextMoving_], animation_[nextMoving_].duration);
		transitionAnimationT_ += transitionSpeed_;
		AnimationManager::LerpSkeleton(transitionAnimationT_, skeleton_, animation_[currentMoving_], animationTime_[currentMoving_], animation_[nextMoving_], animationTime_[nextMoving_]);
		if (transitionAnimationT_ >= 1.0f) {
			animationTime_[currentMoving_] = 0.0f;
			transitionAnimationT_ = 0.0f;
			currentMoving_ = nextMoving_;
		}
	}
	else {
		animationTime_[currentMoving_] += 1.0f / 60.0f;
		animationTime_[currentMoving_] = std::fmod(animationTime_[currentMoving_], animation_[currentMoving_].duration);
		AnimationManager::ApplyAnimation(skeleton_, animation_[currentMoving_], animationTime_[currentMoving_]);
	}
}
