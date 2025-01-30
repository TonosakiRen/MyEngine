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

    enum Moving {
        kWalk,
        kStand,
        kMovingNum
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

    Engine::AnimationManager* animationManager_ = nullptr;
    Engine::ModelManager* modelManager_ = nullptr;
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
};