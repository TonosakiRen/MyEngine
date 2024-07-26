#define WaveRadius 6.0f

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
ConstantBuffer<Frustum> frustum  : register(b4);

struct MeshletInfo {
	uint32_t meshletNum;
};
ConstantBuffer<MeshletInfo> meshletInfo  : register(b3);

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
	uint32_t hitWaveIndices[32];
	uint32_t visibleCount;
};

groupshared Payload payload;

struct WaveIndexData {
	uint32_t waveDataNum;
	uint32_t waveIndex[5];
};
ConstantBuffer<WaveIndexData> waveIndexData: register(b6);

struct WaveData {
	float32_t3 position;
	float32_t t;
};
StructuredBuffer<WaveData> waveData :register(t6);

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

bool IsHit(Sphere a, float32_t3 pos,float32_t radius) {
	float distance = length(a.position - pos);
	float sum = a.radius + radius;

	return distance <= sum;
}

[numthreads(32, 1, 1)]
void main(uint32_t dtid : SV_DispatchThreadID)
{
	bool isVisible = false;
	bool isHitWave = false;
	
	if (dtid < meshletInfo.meshletNum) {
		isVisible = IsVisible(cullData[dtid], worldTransform);
	}

	if (isVisible) {
		uint32_t index = WavePrefixCountBits(isVisible);
		payload.meshletIndices[index] = dtid;
		for (int i = 0; i < waveIndexData.waveDataNum;i++) {
			WaveData wave = waveData[waveIndexData.waveIndex[i]];
			if (IsHit(cullData[dtid].sphere, wave.position, WaveRadius)) {
				isHitWave = true;
			}
		}

		if (isHitWave) {
			index = WavePrefixCountBits(isHitWave);
			payload.hitWaveIndices[index] = dtid;
		}
	}

	uint32_t visibleCount = WaveActiveCountBits(isVisible);
	uint32_t hitCount = WaveActiveCountBits(isHitWave);

	payload.visibleCount = visibleCount;

	DispatchMesh(visibleCount + hitCount, 1, 1, payload);
}