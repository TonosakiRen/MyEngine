struct WorldTransform {
	float32_t4x4 world;
	float32_t4x4 worldInverseTranspose;
	float32_t scale;
};
ConstantBuffer<WorldTransform> worldTransform  : register(b0);

struct Plane {
	float32_t3 normal;
	float32_t distance;
};
struct Frustum {
	Plane plane[6];
};
ConstantBuffer<Frustum> frustum  : register(b3);

struct MeshletInfo {
	uint32_t meshletNum;
};
ConstantBuffer<MeshletInfo> meshletInfo  : register(b2);

struct Sphere {
	float32_t3 position;
	float32_t radius;
};

struct CullData {
	Sphere sphere;
	uint32_t normalCone;
	float32_t ApexOffset;
};
StructuredBuffer<CullData> cullData : register(t5);

struct Payload {
	uint32_t meshletIndices[32];
};
groupshared Payload payload;

float32_t3 Multiply(float32_t4x4 m, float32_t3 vec) {
	float32_t4 result;
	result = mul(float32_t4(vec, 1.0f), m);
	result.x = result.x * rcp(result.w);
	result.y = result.y * rcp(result.w);
	result.z = result.z * rcp(result.w);
	return result.xyz;
}

bool IsVisible(CullData cullData, WorldTransform worldTransform) {
	float32_t3 center = Multiply(worldTransform.world, cullData.sphere.position);
	float32_t radius = cullData.sphere.radius * worldTransform.scale;

	for (uint32_t i = 0; i < 6; i++) {
		float distance = dot(frustum.plane[i].normal, center) - frustum.plane[i].distance;
		if (distance > radius) {
			return false;
		}
	}
	return true;
}

[numthreads(32, 1, 1)]
void main(uint32_t dtid : SV_DispatchThreadID)
{
	bool isVisible = false;
	 
	if (dtid < meshletInfo.meshletNum) {
		isVisible = IsVisible(cullData[dtid], worldTransform);
	}

	if (isVisible) {
		uint32_t index = WavePrefixCountBits(isVisible);
		payload.meshletIndices[index] = dtid;
	}

	uint32_t visibleCount = WaveActiveCountBits(isVisible);
	DispatchMesh(visibleCount, 1, 1, payload);
}