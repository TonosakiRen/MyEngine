#include "LightNumBuffer.h"
#include "DirectionalLights.h"
#include "PointLights.h"
#include "SpotLights.h"
#include "ShadowSpotLights.h"
#include "AreaLights.h"

void LightNumBuffer::Create()
{
	lightNumBuffer_.Create(sizeof(LightNum));
	LightNum lightNum{ DirectionalLights::lightNum,PointLights::lightNum,AreaLights::lightNum,SpotLights::lightNum, ShadowSpotLights::lightNum};
	lightNumBuffer_.Copy(lightNum);
}
