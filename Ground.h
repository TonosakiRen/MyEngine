#pragma once
#include "GameObject.h"
#include "Collider.h"
class Ground :
    public GameObject
{
public:

    void Initialize(ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector3 initialPos = { 1.0f,1.0f,1.0f });
    void Update();
    void Draw();

    WorldTransform* GetWorldTransform() {
        return &worldTransform_;
    }
public:
    Collider collider_;
    bool isMove_ = false;
    float speed_ = -0.05f;
};

