#pragma once
/**
 * @file Mushrooms.h
 * @brief キノコ達
 */
#include <memory>
#include <vector>
#include "GameComponent/GameObject.h"
#include "GameComponent/Material.h"
#include "Particle/ParticleData.h"
#include "Particle/MushroomParticle.h"
#include "Stage/Mushroom.h"
#include "Light/LightManager.h"

class Mushrooms
{
public:
    static const uint32_t kMushroomNum_ = 60;
    static const uint32_t kMushroomParticleNum = MushroomParticle::kParticleNum * kMushroomNum_;
    Mushrooms();
    void Initialize(const std::string& name,WorldTransform& parentWorldTransform);
    void Update();
    void Draw();
    //当たり判定、当たっていたら色を変える
    void ChangeColorSphere(Collider& collider, const Vector4& color);
private:
    std::array<Mushroom, kMushroomNum_> mushrooms;
    std::unique_ptr<ParticleData> particleData_;
    std::unique_ptr<ParticleModelData> data_;
    uint32_t particleTextureHandle_ = 0;
    PointLights* pointLights_;
    std::unique_ptr<Material> material_;
    Vector4 color_;
    uint32_t modelHandle_ = 0;
};