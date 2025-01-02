#pragma once
/**
 * @file PlayerModel.h
 * @brief PlayerのModel
 */
#include <memory>
#include "GameComponent/GameObject.h"
#include "Input.h"
#include "Mymath.h"
#include "Animation/AnimationManager.h"
#include "Model/ModelManager.h"
#include "Animation/SkinCluster.h"

class PlayerModel 
{
public:

    enum ModelJoints {
        HeadTop,
        Head,

        SpineTop,

        RightShoulder,
        RightArm,
        RightForArm,
        RightHand,

        LeftShoulder,
        LeftArm,
        LeftForArm,
        LeftHand,

        SpineBottom,

        RightUpLeg,
        RightLeg,
        RightFoot,

        LeftUpLeg,
        LeftLeg,
        LeftFoot,

        PartNum
    };

    enum Moving {
        kWalk,
        kStand,
        kMovingNum
    };

    struct JointWorldTransform {
        const Matrix4x4* skeletonSpaceMatrix;
        WorldTransform worldTransform;
    };

    static const int fireNum_ = 40;

    void Initialize(WorldTransform& worldTransform);
    void Update();
    void Draw();

    //動きをセットする
    void SetMoving(Moving moving) {
        if (transitionAnimationT_ == 0.0f) {
            nextMoving_ = moving;
        }
    }

private:
    //Animation更新
    void AnimationUpdate();

    AnimationManager* animationManager_ = nullptr;
    ModelManager* modelManager_ = nullptr;
    Moving currentMoving_ = kWalk;
    Moving nextMoving_ = kWalk;
    float animationTime_[kMovingNum];
    Animation animation_[kMovingNum];
    float transitionAnimationT_ = 0.0f;
    float transitionSpeed_ = 0.1f;
    Skeleton skeleton_;
    SkinCluster skinCluster_;

    WorldTransform* playerWorldTransform_ = nullptr;
    uint32_t modelHandle_ = 0;
    std::array<JointWorldTransform, PartNum> jointWordTransforms_;
    std::array<JointWorldTransform, PartNum> partsWordTransform_;
};