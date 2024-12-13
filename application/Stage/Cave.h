#pragma once
/**
 * @file Cave.h
 * @brief 洞窟
 */
#include "GameComponent/GameObject.h"
#include "Stage/Mushrooms.h"
class Cave :
    public GameObject
{
public:

    void Initialize();
    void Update();
    void Draw();

    std::unique_ptr < Mushrooms>& GetMushrooms() {
        return mushRooms_;
    }
private:
    std::unique_ptr<Mushrooms> mushRooms_;
};