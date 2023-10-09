#include "Transform.h"
void Transform::UpdateMatrix() {
    worldMatrix = MakeAffineMatrix(scale, rotate, translate);
    if (parent) {
        worldMatrix *= parent->worldMatrix;
    }
}