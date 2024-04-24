#pragma once
#include "GameObject.h"
class Floor :
    public GameObject
{
public:

    void Initialize(const std::string name);
    void Update();
    void Draw();
    float size_ = 50.0f;
    uint32_t textureHandle_ = 0;
};