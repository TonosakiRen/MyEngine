#pragma once
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
    float size_ = 50.0f;
    uint32_t textureHandle_ = 0;
};