#include "ViewProjection.h"
#include "WinApp.h"
#include "Input.h"
#include "ImGuiManager.h"


bool ViewProjection::isUseDebugCamera = false;

void ViewProjection::SwitchIsUseDebugCamera()
{
#ifdef _DEBUG
    ImGui::Begin("Game");
    if (ImGui::BeginMenu("Camera")) {
        if (ImGui::Button("SwitchDebugCamera")) {
            if (ViewProjection::isUseDebugCamera == false) {
                ViewProjection::isUseDebugCamera = true;
            }
            else {
                ViewProjection::isUseDebugCamera = false;
            }
        }
        ImGui::EndMenu();
    }
    ImGui::End();
#endif 
}

void ViewProjection::Initialize() {
    constBuffer_.Create(sizeof(ConstBufferData));
    Update();
}

void ViewProjection::Update() {

    // ビュー行列の生成
    Vector3 tranlation = translation_ + Vector3{ Rand(-shakeValue_.x,shakeValue_.x),Rand(-shakeValue_.y,shakeValue_.y) ,Rand(-shakeValue_.z,shakeValue_.z) };
    worldMatrix_ = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, quaternion_, tranlation);
    matView_ = Inverse(worldMatrix_);

    // 透視投影による射影行列の生成
    matProjection_ = MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_);
    //matProjection = MakeOrthograohicmatrix(-16.0f * orthographicValue_, 9.0f * orthographicValue_,16.0f * orthographicValue_, -9.0f * orthographicValue_, 1.0f, farZ_);

    Matrix4x4 viewProjection = matView_ * matProjection_;
    inverseViewProjection_ = Inverse(viewProjection);
    frustum_ = MakeFrustrum(inverseViewProjection_);
    worldFrustum_ = frustum_ * worldMatrix_;

    // 定数バッファに書き込み
    ConstBufferData bufferData;
    bufferData.viewProjection = viewProjection;
    bufferData.inverseViewProjection = inverseViewProjection_;
    bufferData.worldMatrix = worldMatrix_;

    bufferData.billBorMatrix = matView_;
    bufferData.billBorMatrix.m[3][0] = 0.0f;
    bufferData.billBorMatrix.m[3][1] = 0.0f;
    bufferData.billBorMatrix.m[3][2] = 0.0f;
    bufferData.billBorMatrix = Inverse(bufferData.billBorMatrix);
    bufferData.viewPosition = tranlation;

    constBuffer_.Copy(bufferData);
}

bool ViewProjection::Shake(Vector3 shakeValue, int& frame)
{
    frame--;
    if (frame > 0) {
        shakeValue_ = shakeValue;
        return true;
    }
    frame = 0;
    shakeValue_ = { 0.0f,0.0f,0.0f };
    return false;
}