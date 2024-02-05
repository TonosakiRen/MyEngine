#pragma once
#include "GameObject.h"
#include "Input.h"
#include "Collider.h"
#include "Sprite.h"
#include "Mymath.h"
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
    Sprite sprite2DReticle_;
    WorldTransform modelWorldTransform_;
    WorldTransform leftHandWorldTransform_;
    WorldTransform rightHandWorldTransform_;
    Vector3 velocity_;
    Vector3 acceleration_;
    Vector3 modelSize_;
    Vector4 color_;
};