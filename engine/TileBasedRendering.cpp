#include "TileBasedRendering.h"
#include "DirectXCommon.h"

void TileBasedRendering::Initialize()
{
    {
        // インスタンシングデータのサイズ
        UINT sizeINB = static_cast<UINT>(sizeof(ConstBufferData) * kTileNum);
        structureBuffer_.Create(sizeINB, kTileNum);
    }

    {
        // インスタンシングデータのサイズ
        UINT sizeINB = static_cast<UINT>(sizeof(uint32_t) * (kTileNum * PointLights::lightNum));
        pointLightIndexBuffer_.Create(sizeINB, kTileNum * PointLights::lightNum);
    }

    {
        // インスタンシングデータのサイズ
        UINT sizeINB = static_cast<UINT>(sizeof(uint32_t) * (kTileNum * SpotLights::lightNum));
        spotLightIndexBuffer_.Create(sizeINB, kTileNum * SpotLights::lightNum);

    }

    {
        // インスタンシングデータのサイズ
        UINT sizeINB = static_cast<UINT>(sizeof(uint32_t) * (kTileNum * SpotLights::lightNum));
        shadowSpotLightIndexBuffer_.Create(sizeINB, kTileNum * SpotLights::lightNum);
    }

}

void TileBasedRendering::Update(const PointLights& pointLights,const SpotLights& spotLights, const ShadowSpotLights& shadowSpotLights)
{
    //初期化
    for (int i = 0; i < kTileNum; i++) {
        tilesInformation_[i].pointLightNum = 0;
        tilesInformation_[i].spotLightNum = 0;
        tilesInformation_[i].shadowSpotLightNum = 0;
        tilesInformation_[i].pointLightIndex.clear();
        tilesInformation_[i].spotLightIndex.clear();
        tilesInformation_[i].shadowSpotLightIndex.clear();
    }


    for (uint32_t i = 0; i < PointLights::lightNum;i++) {
        if (pointLights.lights_[i].isActive) {
            for (uint32_t j = 0; j < kTileNum; j++) {
                tilesInformation_[j];
                pointLights.lights_[i];
                //当たった
                if (true) {
                    tilesInformation_[j].pointLightNum++;
                    tilesInformation_[j].pointLightIndex.push_back(i);
                }
            }
        }
    }

    for (uint32_t i = 0; i < SpotLights::lightNum; i++) {
        if (spotLights.lights_[i].isActive) {
            for (uint32_t j = 0; j < kTileNum; j++) {
                tilesInformation_[j];
                spotLights.lights_[i];
                //当たった
                if (true) {
                    tilesInformation_[j].spotLightNum++;
                    tilesInformation_[j].spotLightIndex.push_back(i);
                }
            }
        }
    }

    for (uint32_t i = 0; i < ShadowSpotLights::lightNum; i++) {
        if (shadowSpotLights.lights_[i].isActive) {
            for (uint32_t j = 0; j < kTileNum; j++) {
                tilesInformation_[j];
                shadowSpotLights.lights_[i];
                //当たった
                if (true) {
                    tilesInformation_[j].shadowSpotLightNum++;
                    tilesInformation_[j].shadowSpotLightIndex.push_back(i);
                }
            }
        }
    }

    //コピー
    std::vector<ConstBufferData> bufferData;
    bufferData.reserve(kTileNum);

    uint32_t pointLightIndex[kTileNum * PointLights::lightNum ];
    uint32_t spotLightLightIndex[kTileNum * SpotLights::lightNum ];
    uint32_t shadowSpotLightLightIndex[kTileNum * ShadowSpotLights::lightNum];
    

    for (uint32_t i = 0; i < kTileNum; i++) {
        ConstBufferData data;
      
        data.pointLightNum = tilesInformation_[i].pointLightNum;
        data.spotLightNum = tilesInformation_[i].spotLightNum;
        data.shadowSpotLightNum = tilesInformation_[i].shadowSpotLightNum;

        for (int j = 0; uint32_t index : tilesInformation_[i].pointLightIndex) {
            pointLightIndex[i * PointLights::lightNum + j] = index;
            j++;
        }
        for (int j = 0; uint32_t index : tilesInformation_[i].spotLightIndex) {
            spotLightLightIndex[i * SpotLights::lightNum + j] = index;
            j++;
        }
        for (int j = 0; uint32_t index : tilesInformation_[i].shadowSpotLightIndex) {
            shadowSpotLightLightIndex[i * ShadowSpotLights::lightNum + j] = index;
            j++;
        }
        bufferData.emplace_back(data);
    }

    structureBuffer_.Copy(bufferData.data(), sizeof(bufferData[0]) * bufferData.size());
    pointLightIndexBuffer_.Copy(pointLightIndex);
    spotLightIndexBuffer_.Copy(spotLightLightIndex);
    shadowSpotLightIndexBuffer_.Copy(shadowSpotLightLightIndex);
}
