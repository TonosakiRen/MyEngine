#pragma once
#include "GameObject.h"
#include "Input.h"
#include "ParticleBox.h"
#include "DustParticle.h"
#include "Collider.h"
class Player :
    public GameObject
{
public:

    void Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight);
    void Update();
    void Move();
    void Attack();
    void Collision(Collider& blockCollider);
    void Animation();
    void Draw();
    void ParticleDraw();
    void SetInitialPos();

    WorldTransform* GetWorldTransform() {
        return &worldTransform_;
    }
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
    Vector3 velocity_;
    Vector3 accelaration_;
    bool isJump_ = false;
    bool isGround_ = false;
    bool isWalking_ = false;

    //animation
    float animationT_ = 0.0f;
    float animationBodyT_ = 0.0f;
    float animationSpeed_ = 0.05f;
    float animationBodySpeed_ = animationSpeed_ * 2.0f;
    float runUpAnimation_ = 0.08f;

    //particle
    std::unique_ptr<DustParticle> dustParticle_;

    //武器
    WorldTransform weaponRotateWorldTransform_;
    GameObject weaponObject_;
    float weaponDegree_ = 0.0f;

    //block,png
    uint32_t blockHandle_;

public:
    //collider
    Collider collider;
    Collider weaponCollider_;

    bool isGrounding_ = false;
};