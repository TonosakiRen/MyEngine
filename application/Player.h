#pragma once
#include "GameObject.h"
#include "Input.h"
#include "Collider.h"
#include "Sprite.h"
#include "Mymath.h"
#include "AnimationManager.h"
#include "ModelManager.h"

class PlayerBulletManager;
class Player :
    public GameObject
{
public:

    void Initialize(const std::string name,PlayerBulletManager* playerBulletManager);
    void Update(const ViewProjection& viewProjection);
    void Extrusion(Collider& otherCollider);
    void OnCollision();
    void Draw();
    void ReticleDraw();
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
    Sprite sprite2DReticle_;
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
};