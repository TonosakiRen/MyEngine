#pragma once
/**
 * @file LightObject.h
 * @brief ライトを持ったオブジェクト
 */
#include <memory>
#include <vector>
#include "GameComponent/GameObject.h"
#include "GameComponent/Collider.h"
#include "Particle/ParticleModelData.h"
#include "Light/LightManager.h"
class LightObject : public GameObject
{
friend class LightObjects;
public:
    void Initialize(const std::string& name, PointLight* pointLight, const Vector4& color, const Vector3& scale, const Quaternion& quaternion, const Vector3& position);
    void Update();

    //Setter
    void SetColor(const Vector4& color);
    //Getter
    Collider& GetCollider() { return collider_; }
protected:
    Collider collider_;
    Vector4 color_;
    PointLight* pointLight_ = nullptr;
    float intensity_ = 3.0f;
    bool isActive_ = false;
};