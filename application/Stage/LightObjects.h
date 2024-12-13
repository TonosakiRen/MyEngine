#pragma once
/**
 * @file LightObjects.h
 * @brief ライトを持ったオブジェクトを複数持ったもの
 */
#include <memory>
#include <vector>
#include "GameComponent/GameObject.h"
#include "GameComponent/Material.h"
#include "Particle/ParticleModelData.h"
#include "Stage/LightObject.h"
#include "Light/LightManager.h"

class LightObjects
{
public:
    static const uint32_t kObjectNum_ = 128;
    void Initialize(const std::string& name);
    void Update();
    void Draw();
private:
    std::array<LightObject, kObjectNum_> objects_;
    std::unique_ptr<ParticleModelData> data_;
    PointLights* pointLights_;
    std::unique_ptr<Material> material_;
    Vector4 color_;
};