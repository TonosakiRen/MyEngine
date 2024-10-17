#include "Light/PointLights.h"
#include "Graphics/DirectXCommon.h"

void PointLights::Initialize() {
    
    structureBuffer_.Create(L"pointLightBuffer", sizeof(ConstBufferData), lightNum);
    lights_.resize(lightNum);
}

void PointLights::Update() {
    std::vector<ConstBufferData> bufferData;
    bufferData.reserve(lightNum);

    assert(lights_.size() <= lightNum);

    for (int i = 0; i < lights_.size(); i++) {
        ConstBufferData data;
        data.color = lights_[i].color;
        lights_[i].worldTransform.Update();
        data.position = MakeTranslation(lights_[i].worldTransform.matWorld_);
        data.intensity = lights_[i].intensity;
        data.radius = lights_[i].radius;
        data.decay = lights_[i].decay;
        data.isActive = static_cast<float>(lights_[i].isActive);
        bufferData.emplace_back(data);
    }

    structureBuffer_.Copy(bufferData.data(), sizeof(bufferData[0]) * bufferData.size());
}
