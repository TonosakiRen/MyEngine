/**
 * @file LightNumBuffer.h
 * @brief Lightの数を管理
 */
#include "Light/LightNumBuffer.h"
#include "Light/DirectionalLights.h"
#include "Light/PointLights.h"
#include "Light/SpotLights.h"
#include "Light/ShadowSpotLights.h"
#include "Light/AreaLights.h"
#include "Light/TileBasedRendering.h"

void LightNumBuffer::Initialize()
{
	lightNumBuffer_.Create(L"LightNumBuffer",sizeof(LightNum));
	LightNum lightNum{ DirectionalLights::lightNum,PointLights::lightNum,AreaLights::lightNum,SpotLights::lightNum, ShadowSpotLights::lightNum,TileBasedRendering::kMaxInTilePointLight };
	lightNumBuffer_.Copy(lightNum);
}