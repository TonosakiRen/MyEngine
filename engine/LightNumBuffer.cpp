#include "LightNumBuffer.h"
#include "DirectionalLights.h"
#include "PointLights.h"
#include "SpotLights.h"
#include "ShadowSpotLights.h"
#include "AreaLights.h"
#include "TileBasedRendering.h"

void LightNumBuffer::Initialize()
{
	lightNumBuffer_.Create(L"LightNumBuffer",sizeof(LightNum));
	LightNum lightNum{ DirectionalLights::lightNum,PointLights::lightNum,AreaLights::lightNum,SpotLights::lightNum, ShadowSpotLights::lightNum,TileBasedRendering::kMaxInTilePointLight };
	lightNumBuffer_.Copy(lightNum);
}