#pragma once
/**
 * @file Tree.h
 * @brief 木
 */
#include <memory>
#include <vector>
#include "GameComponent/GameObject.h"
#include "Particle/ParticleModelData.h"
class Tree :
    public GameObject
{
public:
    void Initialize();
    void Update();
    void Draw();
private:
    std::vector<WorldTransform> grassTransforms_;
    std::unique_ptr<ParticleModelData> grassesData_;
    std::unique_ptr<Material> grassMaterial_;
};