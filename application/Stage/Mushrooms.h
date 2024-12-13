#pragma once
/**
 * @file Mushrooms.h
 * @brief キノコ達
 */
#include <memory>
#include <vector>
#include "GameComponent/GameObject.h"
#include "GameComponent/Material.h"
#include "Particle/ParticleModelData.h"
#include "Stage/Mushroom.h"
#include "Light/LightManager.h"

class Mushrooms
{
public:
    static const uint32_t kMushroomNum_ = 82;
    void Initialize(const std::string& name,WorldTransform& parentWorldTransform);
    void ChangeColorSphere(Collider& collider, const Vector4& color);
    void Update();
    void Draw();
private:
    std::array<Mushroom, kMushroomNum_> mushrooms;
    std::unique_ptr<ParticleModelData> data_;
    PointLights* pointLights_;
    std::unique_ptr<Material> material_;
    Vector4 color_;
    uint32_t modelHandle_ = 0;
};