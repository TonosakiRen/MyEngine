#pragma once

#include "MyMath.h"

class Camera {
public:
    enum ProjectionType {
        Perspective,
        Orthographic
    };

    Camera(ProjectionType projectionType = Perspective);

    void UpdateMatrices();

    void SetPosition(const Vector3& position) { position_ = position, needUpdateing_ = true; }
    void SetRotate(const Vector3& rotate) { rotate_ = rotate, needUpdateing_ = true; }

    void SetPerspective(float fovY, float aspectRaito, float nearClip, float farClip);
    void SetOrthographic(float width, float height, float nearClip, float farClip);

    float GetNearClip() const { return nearClip_; }
    float GetFarClip() const { return farClip_; }
    const Vector3& GetPosition() const { return position_; }
    const Vector3& GetRotate() const { return rotate_; }
    const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
    const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
    const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }

private:
    Vector3 position_;
    Vector3 rotate_;

    ProjectionType projectionType_;
    union Projection {
        struct Perspective {
            float fovY;
            float aspectRaito;
        } perspective;
        struct Orthographic {
            float width;
            float height;
        } orthographic;
    };
    
    Projection projection_;

    float nearClip_;
    float farClip_;

    Matrix4x4 viewMatrix_;
    Matrix4x4 projectionMatrix_;
    Matrix4x4 viewProjectionMatrix_;

    bool needUpdateing_;
};