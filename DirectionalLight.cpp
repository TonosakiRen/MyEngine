#include "DirectionalLight.h"

void DirectionalLight::Initialize() {
    constBuffer_.Create((sizeof(ConstBufferData) + 0xff) & ~0xff);
    Update();
}

void DirectionalLight::Update() {

    // 定数バッファに書き込み
    ConstBufferData bufferData;
    bufferData.color = color_;
    bufferData.direction = Normalize(direction_);
    bufferData.intensity = intensity_;

    constBuffer_.Copy(bufferData);
}