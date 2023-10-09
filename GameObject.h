#pragma once

#include <memory>
#include <vector>
#include <string>

#include "MyMath.h"
#include "Transform.h"
#include "Camera.h"
#include "Model.h"


class GameObject {
public:
    void Initialize(std::string name);
    void Update();
    void Draw(CommandContext& commandContext, const Camera& camera);

private:
    Transform transform_;
    Model model_;

};