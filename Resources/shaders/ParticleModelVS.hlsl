#include "Common.hlsli"
struct ParticleData {
	float32_t4x4 world;
	float32_t4x4 worldInverseTranspose;
	float32_t4 color;
};
StructuredBuffer<ParticleData> gParticleData  : register(t1);
ConstantBuffer<ViewProjection> gViewProjection  : register(b0);

struct VSInput {
	float32_t3 pos : POSITION;
	float32_t3 normal : NORMAL;
	float32_t2 uv : TEXCOORD;
	uint32_t instanceId : SV_InstanceID;
};

struct VSOutput {
	float32_t4 svpos : SV_POSITION;
	float32_t3 normal : NORMAL;
	float32_t2 uv : TEXCOORD;
	float32_t3 worldPosition : POSITION0;
	float32_t depth : TEXCOORD1;
	uint32_t instanceId : TEXCOORD2;
};

VSOutput main(VSInput input) {
	VSOutput output;

	float32_t4 worldPosition = mul(float4(input.pos, 1.0f), gParticleData[input.instanceId].world);
	float32_t4 viewPosition = mul(worldPosition, gViewProjection.viewProjection);

	output.svpos = viewPosition;
	output.normal = mul(input.normal, (float32_t3x3)gParticleData[input.instanceId].worldInverseTranspose);
	output.uv = input.uv;
	output.worldPosition = worldPosition.xyz;
	output.depth = viewPosition.z / viewPosition.w;
	output.instanceId = input.instanceId;


	return output;
}