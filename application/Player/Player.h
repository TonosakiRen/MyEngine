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
    static const int fireNum_ = 40;

    void Initialize(const std::string name);
    void Update(const ViewProjection& viewProjection);
    void Draw();
    //押し出し当たり判定
    void Extrusion(Collider& otherCollider);
    //当たり判定
    void OnCollision();
    //Setter
    void SetColor(const Vector4& color);
    void SetBulletManager(PlayerBulletManager* playerBulletManager) {
        playerBulletManager_ = playerBulletManager;
    }
    //Getter
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
    //弾発射
    void Fire();
    //動き
    void Move(const ViewProjection& viewProjection);
    //レティクルアップデート
    void ReticleUpdate(const ViewProjection& viewProjection);
private:
    const float growSpeed = 0.01f;

    Engine::Input* input_;
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

    SpriteData spriteData_;
};