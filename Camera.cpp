#include "Camera.h"
#include <cmath>
Camera::Camera(ProjectionType projectionType) {
    projectionType_ = projectionType;
    position_ = { 0.0f,0.0f, -6.0f };
    rotate_ = {0.0f,0.0f,0.0f};

    projection_.perspective.fovY = 45.0f * M_PI;
    projection_.perspective.aspectRaito = 1280.0f / 720.0f;
    nearClip_ = 0.1f;
    farClip_ = 100.0f;

    needUpdateing_ = true;

    UpdateMatrices();
}

void Camera::UpdateMatrices() {
    if (needUpdateing_) {
        viewMatrix_ = MakeViewMatirx(rotate_, position_);

        switch (projectionType_)
        {
        default:
        case Camera::Perspective:
            projectionMatrix_ = MakePerspectiveFovMatrix(projection_.perspective.fovY, projection_.perspective.aspectRaito, nearClip_, farClip_);
            break;
        case Camera::Orthographic:
            projectionMatrix_ = MakeOrthograohicmatrix(projection_.orthographic.width, projection_.orthographic.height, nearClip_, farClip_);
            break;
        }

        viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
    }
}

void Camera::SetPerspective(float fovY, float aspectRaito, float nearClip, float farClip) {
    projection_.perspective.fovY = fovY;
    projection_.perspective.aspectRaito = aspectRaito;
    nearClip_ = nearClip;
    farClip_ = farClip;
    projectionType_ = Perspective;
    needUpdateing_ = true;
}

void Camera::SetOrthographic(float width, float height, float nearClip, float farClip) {
    projection_.orthographic.width = width;
    projection_.orthographic.height = height;
    nearClip_ = nearClip;
    farClip_ = farClip;
    projectionType_ = Orthographic;
    needUpdateing_ = true;
}
