#pragma once
#include "GameObject.h"
class Floor :
    public GameObject
{
public:
    static Floor* Create(const std::string name);

    void Update();
};

