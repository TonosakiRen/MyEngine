#pragma once
/**
 * @file Player.h
 * @brief player
 */
#include <memory>
#include "GameComponent/GameObject.h"
#include "Input.h"
#include "GameComponent/Collider.h"
#include "Texture/SpriteData.h"
#include "Mymath.h"
#include "Animation/AnimationManager.h"
#include "Model/ModelManager.h"
#include "Animation/SkinCluster.h"
#include "Particle/DustParticle.h"
#include "PlayerModel.h"
#include "Light/LightManager.h"

class PlayerBulletManager;
class Player :
    public GameObject
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

    struct JointWorldTransform {
        const Matrix4x4* skeletonSpaceMatrix;
        WorldTransform worldTransform;
    };

    static const int fireNum_ = 40;

    void Initialize(const std::string name,PlayerBulletManager* playerBulletManager);
    void Update(const ViewProjection& viewProjection);
    void Extrusion(Collider& otherCollider);
    void OnCollision();
    void SetColor(const Vector4& color);
    void Draw();
    void Fire();
    void Move(const ViewProjection& viewProjection);
    void ReticleUpdate(const ViewProjection& viewProjection);
    const WorldTransform& GetWorldTransform() const {
        return worldTransform_;
    }
    Collider& GetSphereCollider() {
        return lightCollider_;
    }
    const Vector4& GetColor() {
        return color_;
    }
public:
    Collider collider_;
private:
    const float growSpeed = 0.01f;

    Input* input_;
    PointLight* pointLight_ = nullptr;
    PlayerBulletManager* playerBulletManager_;
    PlayerModel playerModel_;
    WorldTransform worldTransform3DReticle_;
    Quaternion inputQuaternion_;
    SpriteData sprite2DReticle_;

    Vector3 velocity_;
    Vector3 acceleration_;
    Vector3 modelSize_;
    Vector4 color_;
    Vector3 direction_;
    float colorT_ = 1.0f;

   
    std::unique_ptr<DustParticle> fireParticle_;

    bool isGrowSphere_ = false;
    Collider lightCollider_;
    float lightSphereT_ = 0.0f;

};