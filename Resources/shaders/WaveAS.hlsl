#include "Common.hlsli"
struct Payload {
	uint32_t meshletIndices[32];
	uint32_t hitWaveIndices[32];
	uint32_t visibleCount;
};
groupshared Payload payload;
ConstantBuffer<WorldTransform> worldTransform  : register(b0);
ConstantBuffer<MeshletInfo> meshletInfo  : register(b3);
ConstantBuffer<Frustum> frustum  : register(b4);
ConstantBuffer<WaveParam> waveParam: register(b7);

StructuredBuffer<CullData> cullData : register(t5);
StructuredBuffer<WaveData> waveData :register(t6);
StructuredBuffer<uint32_t> waveIndexData: register(t7);

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
		
		uint32_t index;

		isHitWave = true;

		for (int i = 0; i < waveIndexData[0];i++) {
			WaveData wave = waveData[waveIndexData[i + 1]];
			if (IsHit(cullData[dtid].sphere, wave.position, waveParam.radius)) {
				isHitWave = true;
			}
		}

		if (isHitWave) {
			index = WavePrefixCountBits(isHitWave);
			payload.hitWaveIndices[index] = dtid;
		}else{
			index = WavePrefixCountBits(isVisible);
			payload.meshletIndices[index] = dtid;
		}
	}

	uint32_t visibleCount = WaveActiveCountBits(isVisible);
	uint32_t hitCount = WaveActiveCountBits(isHitWave);

	payload.visibleCount = visibleCount - hitCount;
	//payload.hitCount = hitCount;

	DispatchMesh(visibleCount - hitCount + hitCount * 4, 1, 1, payload);
}