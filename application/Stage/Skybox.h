#pragma once
#include "GameComponent/GameObject.h"
class Skybox :
    public GameObject
{
public:

    void Initialize(const std::string name);
    void Update();
    void Draw();
};