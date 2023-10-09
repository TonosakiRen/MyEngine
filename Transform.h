#pragma once

#include "MyMath.h"

class Transform {
public:
    void UpdateMatrix();

    Vector3 scale = {0.0f,0.0f,0.0f};
    Vector3 rotate;
    Vector3 translate;
    Matrix4x4 worldMatrix;
    const Transform* parent = nullptr;
};