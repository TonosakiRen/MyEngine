#pragma once
#include "GameObject.h"
class Skydome :
    public GameObject
{
public: 
    static Skydome* Create(const std::string name);

    void Update();
};

