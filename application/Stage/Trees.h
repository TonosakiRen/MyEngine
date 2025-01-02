#pragma once
/**
 * @file Trees.h
 * @brief 木々
 */
#include <memory>
#include <vector>
#include "GameComponent/GameObject.h"
#include "Particle/ParticleModelData.h"
#include "Stage/Tree.h"
class Trees
{
public:
    static const uint32_t kTreeNum_ = 2 * 4;
    void Initialize();
    void Update();
    void Draw();
public:
    std::vector<Tree> trees_;
};