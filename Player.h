#pragma once
#include "GameObject.h"
#include "Input.h"
#include "ParticleBox.h"
class Player :
    public GameObject
{
public:
   
    void Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight , int particleNum);
    void Update();
    void Animation();
    void Draw();
    void ParticleDraw();
public:

private:
    Input* input_ = nullptr;
    enum parts {
        LeftArm,
        RightArm,
        LeftLeg,
        RightLeg,

        partNum
    };
    Model modelParts_;
    WorldTransform partsTransform_[partNum];
    Vector2 velocity_;
    Vector2 accelaration_;
    bool isJump_ = false;

    //animation
    float animationT_ = 0.0f;
    float animationBodyT_ = 0.0f;
    float animationSpeed_ = 0.05f;
    float animationBodySpeed_ = animationSpeed_ * 2.0f;
    float runUpAnimation_ = 0.08f;

    //particle
    std::unique_ptr<ParticleBox> particleBox_;
    Vector3 translation;
};

