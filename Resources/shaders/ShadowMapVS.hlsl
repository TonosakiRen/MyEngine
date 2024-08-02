#include "Common.hlsli"
#include "Lighting.hlsli"
ConstantBuffer<WorldTransform> gWorldTransform  : register(b0);
ConstantBuffer<DirectionalLight> gDirectionLight  : register(b1);

struct VSOutput {
	float32_t4 svpos : SV_POSITION;
};

VSOutput main(float32_t3 pos : POSITION)
{
	VSOutput output;
	output.svpos = mul(float32_t4(pos,1.0f), mul(gWorldTransform.world, gDirectionLight.viewProjection));

	return output;
}