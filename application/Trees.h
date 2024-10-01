#pragma once
#include <memory>
#include <vector>
#include "GameObject.h"
#include "ParticleModelData.h"
#include "Tree.h"
class Trees
{
private:
    static const uint32_t kTreeNum_ = 2 * 4;

public:
    std::vector<Tree> trees_;
    void Initialize();
    void Update();
    void Draw();
};