#pragma once
/**
 * @file Skybox.h
 * @brief skybox
 */
#include "GameComponent/GameObject.h"
class Skybox :
    public GameObject
{
public:

    void Initialize(const std::string name);
    void Update();
    void Draw();
};