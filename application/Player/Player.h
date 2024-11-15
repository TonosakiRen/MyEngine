#pragma once
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

class PlayerBulletManager;
class Player :
    public GameObject
{
public:

    bool a = false;

    static const int fireNum_ = 40;

    void Initialize(const std::string name,PlayerBulletManager* playerBulletManager);
    void Update(const ViewProjection& viewProjection);
    void Extrusion(Collider& otherCollider);
    void OnCollision();
    void Draw();
    void Fire();
    void Animate();
    void Move(const ViewProjection& viewProjection);
    void ReticleUpdate(const ViewProjection& viewProjection);
    const WorldTransform& GetWorldTransform() const {
        return worldTransform_;
    }
public:
    Collider collider_;
private:
    Input* input_;
    PlayerBulletManager* playerBulletManager_;
    WorldTransform worldTransform3DReticle_;
    Quaternion inputQuaternion_;
    SpriteData sprite2DReticle_;
    WorldTransform modelWorldTransform_;
    GameObject rightHand_;
    Vector3 velocity_;
    Vector3 acceleration_;
    Vector3 modelSize_;
    Vector4 color_;
    Vector3 direction_;
    float animationTime_ = 0.0f;
    float useAnimationTime_ = 0.0f;
    Animation animation_;
    Skeleton skeleton_;
    bool isAnimation_ = false;
    bool isFire_ = false;



    SkinCluster skinCluster_;
    WorldTransform leftHandWorldTransform_;
    WorldTransform leftHandModelWorldTransform_;
    std::unique_ptr<DustParticle> fireParticle_;

    WorldTransform rightHandWorldTransform_;

    SpriteData spriteData_;


};