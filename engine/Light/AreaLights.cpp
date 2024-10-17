#include "Light/AreaLights.h"
#include "Graphics/DirectXCommon.h"

void AreaLights::Initialize() {
    
     lights_.resize(lightNum);
     structureBuffer_.Create(L"areaLightBuffer", sizeof(ConstBufferData), lightNum);
}

void AreaLights::Update() {
    std::vector<ConstBufferData> bufferData;
    bufferData.reserve(lightNum);
    for (int i = 0; i < lightNum; i++) {
        ConstBufferData data;
        data.color = lights_[i].color;
        data.origin = lights_[i].segment.origin;
        data.diff = lights_[i].segment.diff;
        data.intensity = lights_[i].intensity;
        data.range = lights_[i].range;
        data.decay = lights_[i].decay;
        data.isActive = static_cast<float>(lights_[i].isActive);
        bufferData.emplace_back(data);
    }
    structureBuffer_.Copy(bufferData.data(), sizeof(bufferData[0]) * bufferData.size());
}
