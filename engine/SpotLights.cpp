#include "SpotLights.h"
#include "DirectXCommon.h"

void SpotLights::Initialize() {
   
    structureBuffer_.Create(L"pointLightBuffer", sizeof(ConstBufferData), lightNum);
    lights_.resize(lightNum);
    // インスタンシングデータのサイズ
    UINT sizeINB = static_cast<UINT>(sizeof(ConstBufferData) * lightNum);
    Update();
}

void SpotLights::Update() {

    std::vector<ConstBufferData> bufferData;
    bufferData.reserve(lightNum);
    for (int i = 0; i < lightNum; i++) {
        ConstBufferData data;
        data.color = lights_[i].color;
        lights_[i].worldTransform.Update();
        data.position = MakeTranslation(lights_[i].worldTransform.matWorld_);
        data.intensity = lights_[i].intensity;
        data.direction = lights_[i].direction;
        data.distance = lights_[i].distance;
        data.decay = lights_[i].decay;
        data.cosAngle = lights_[i].cosAngle;
        data.isActive = static_cast<float>(lights_[i].isActive);
        bufferData.emplace_back(data);
    }
    structureBuffer_.Copy(bufferData.data(), sizeof(bufferData[0]) * bufferData.size());
}
