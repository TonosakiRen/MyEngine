#include "Light/DirectionalLights.h"
#include "Graphics/DirectXCommon.h"

void DirectionalLights::Initialize() {
    if (lightNum != 0) {
        lights_.resize(lightNum);
        for (int i = 0; i < lightNum; i++) {
            lights_[i].shadowMap_.Create(L"shadowTexture", shadowWidth, shadowHeight, DXGI_FORMAT_D32_FLOAT);
            lights_[i].constBuffer_.Create(L"directionalLightBuffer", (sizeof(ConstBufferData) + 0xff) & ~0xff);
            lights_[i].descriptorHeapIndex = lights_[i].shadowMap_.GetSRV().GetIndex();
        }
        structureBuffer_.Create(L"directionalLightBuffer", sizeof(ConstBufferData), lightNum);
        Update();
    }
}

void DirectionalLights::Update() {

    // 定数バッファに書き込み
    std::vector<ConstBufferData> bufferData;
    bufferData.reserve(lightNum);

    for (int i = 0; i < lightNum; i++) {

        Matrix4x4 viewMatrix = MakeLookRotationMatrix(lights_[i].direction, lights_[i].lockUp);
        viewMatrix.m[3][0] = lights_[i].position.x;
        viewMatrix.m[3][1] = lights_[i].position.y;
        viewMatrix.m[3][2] = lights_[i].position.z;
        viewMatrix = Inverse(viewMatrix);

        ConstBufferData data;
        data.color = lights_[i].color;
        data.direction = Normalize(lights_[i].direction);
        data.intensity = lights_[i].intensity;
        data.viewProjection = viewMatrix * MakePerspectiveFovMatrix(fovAngleY_, aspectRatio_, nearZ_, farZ_);
        data.descriptorHeapIndex = lights_[i].descriptorHeapIndex;

        lights_[i].constBuffer_.Copy(data);
        bufferData.emplace_back(data);
    }

    structureBuffer_.Copy(bufferData.data(), sizeof(bufferData[0]) * bufferData.size());
}

