#pragma once
#include "GameObject.h"
#include "ParticleBox.h"
#include "DustParticle.h"
#include <optional>
#include "Collider.h"

class Boss :
    public GameObject
{
public:


    Boss(ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector3 pos);
   /* void Initialize(ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector3 pos);*/
    void Update();
    void Animation();
    void OnCollision();
    void Draw();

    bool GetIsDead() {
        return isDead_;
    }
private:
    enum parts {
        Head,
        Tin,

        partNum
    };
    Model modelParts_[partNum];
    WorldTransform partsTransform_[partNum];

    //animation
    float animationT_ = 0.0f;
    float animationSpeed_ = 0.01f;

    Vector3 animationAccelaration_ = { 0.0f,-0.025f,0.0f };
    Vector3 animationVelocity_ = { 0.0f,0.0f,0.0f };
    float jumpPower_ = 0.25f;

    enum AnimationStats {
        HeadJump,
        AllJump,
        HeadDown,
        AllDown
    };

    AnimationStats animationStats = HeadJump;

    bool isDead_ = false;

public:
    Collider collider_;

};

