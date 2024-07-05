
struct ParticleData {
	float32_t4x4 world;
	float32_t4x4 worldInverseTranspose;
};
StructuredBuffer<ParticleData> gParticleData  : register(t0);

struct ViewProjection {
	float32_t4x4 viewProjection;
	float32_t4x4 inverseViewProjection;
	float32_t4x4 worldMatrix;
	float32_t4x4 billBordMatrix;
	float32_t3 viewPosition;
};
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
};

VSOutput main(VSInput input) {
	VSOutput output; 
	float32_t4x4 billbordedWorld = gParticleData[input.instanceId].world * gViewProjection.billBordMatrix;
	output.svpos = mul(float32_t4(input.pos, 1.0f), mul(billbordedWorld, gViewProjection.viewProjection));
	output.normal = mul(input.normal, (float32_t3x3)gParticleData[input.instanceId].worldInverseTranspose);
	output.uv = input.uv;
	return output;
}