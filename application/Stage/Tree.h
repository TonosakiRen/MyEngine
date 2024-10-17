#pragma once
#include <memory>
#include <vector>
#include "GameComponent/GameObject.h"
#include "Particle/ParticleModelData.h"
class Tree :
    public GameObject
{
public:
    std::vector<WorldTransform> grassTransforms_;
    std::unique_ptr<ParticleModelData> grassesData_;
    std::unique_ptr<Material> grassMaterial_;
    void Initialize();
    void Update();
    void Draw();
};