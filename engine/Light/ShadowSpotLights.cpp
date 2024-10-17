#include "Light/ShadowSpotLights.h"
#include "Graphics/DirectXCommon.h"
void ShadowSpotLights::Initialize() {
    
    structureBuffer_.Create(L"ShadowSpotLightBuffer", sizeof(ConstBufferData), lightNum);
    lights_.resize(lightNum);

    for (int i = 0; i < lightNum; i++) {
        lights_[i].shadowMap_.Create(L"shadowMapSpotLight", shadowWidth, shadowHeight, DXGI_FORMAT_D32_FLOAT);
        lights_[i].constBuffer_.Create(L"shadowSpotLightBuffer", (sizeof(ConstBufferData) + 0xff) & ~0xff);
        lights_[i].descriptorHeapIndex = lights_[i].shadowMap_.GetSRV().GetIndex();
    }
    Update();
    
}

void ShadowSpotLights::Update() {

    std::vector<ConstBufferData> bufferData;
    bufferData.reserve(lightNum);
    for (int i = 0; i < lightNum; i++) {

        lights_[i].worldTransform.Update();

        Matrix4x4 viewMatrix = MakeLookRotationMatrix(lights_[i].direction, lights_[i].lockUp);
        Vector3 position = MakeTranslation(lights_[i].worldTransform.matWorld_);
        viewMatrix.m[3][0] = position.x;
        viewMatrix.m[3][1] = position.y;
        viewMatrix.m[3][2] = position.z;
        viewMatrix = Inverse(viewMatrix);

        ConstBufferData data;
        data.color = lights_[i].color;
        data.position = position;
        data.intensity = lights_[i].intensity;
        data.direction = lights_[i].direction;
        data.distance = lights_[i].distance;
        data.decay = lights_[i].decay;
        data.cosAngle = lights_[i].cosAngle;
        data.isActive = static_cast<float>(lights_[i].isActive);

        data.viewProjection = viewMatrix * MakePerspectiveFovMatrix(1.0f - lights_[i].cosAngle, aspectRatio_, nearZ_, lights_[i].distance);
        data.descriptorHeapIndex = lights_[i].descriptorHeapIndex;

        lights_[i].constBuffer_.Copy(data);
        bufferData.emplace_back(data);
    }
    structureBuffer_.Copy(bufferData.data(), sizeof(bufferData[0]) * bufferData.size());
}
