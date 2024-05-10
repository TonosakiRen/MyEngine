struct WorldTransform {
	float32_t4x4 world;
	float32_t4x4 worldInverseTranspose;
};
ConstantBuffer<WorldTransform> gWorldTransform  : register(b0);

struct ShadowSpotLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t3 direction;
	float32_t distance;
	float32_t decay;
	float32_t conAngle;
	float32_t isActive;
	uint32_t descriptorIndex;
	float32_t4x4 viewProjection;
};
ConstantBuffer<ShadowSpotLight> gShadowSpotLight  : register(b1);

struct VSOutput {
	float32_t4 svpos : SV_POSITION;
};

VSOutput main(float32_t3 pos : POSITION)
{
	VSOutput output;
	output.svpos = mul(float32_t4(pos, 1.0f), mul(gWorldTransform.world, gShadowSpotLight.viewProjection));

	return output;
}