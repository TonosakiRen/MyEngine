#pragma once
/**
 * @file Mushroom.h
 * @brief キノコ
 */
#include <memory>
#include <vector>
#include "GameComponent/GameObject.h"
#include "GameComponent/Collider.h"
#include "Particle/MushroomParticle.h"
#include "Light/LightManager.h"
#include "Stage/LightObject.h"
class Mushroom : public LightObject
{
friend class Mushrooms;
public:
    void Initialize(const std::string& name, WorldTransform* parentWorldTransform, PointLight* pointLight, const Vector4& color, const float intensityT, const Vector3& scale, const Quaternion& quaternion, const Vector3& position);
    void Update();
    //particleのデータをプッシュする
    void PushDataParticle(std::unique_ptr<ParticleData>& particleData);
    //Setter
    void SetColor(const Vector4& color);
private:
    const Vector4 initColor = { 0.2f,0.2f,1.0f,1.0f };
    MushroomParticle mushroomParticle_;

    float intensityTSpeed_ = 0.01f;
    float colorT_ = 1.0f;
    bool isChangeColor_ = false;
    Vector4 color_ = { 0.2f,0.2f,1.0f,1.0f };
    float intensityT_ = 0.3f;
};