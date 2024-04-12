#pragma once
#include "GameObject.h"
class Skybox :
    public GameObject
{
public:

    void Initialize(const std::string name);
    void Update();
    void Draw();
};