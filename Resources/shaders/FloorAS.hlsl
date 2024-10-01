#include "Common.hlsli"
#include "Lighting.hlsli"
struct Payload {
	uint32_t meshletIndices[32];
	uint32_t hitWaveIndices[32];
	uint32_t visibleCount;
};
groupshared Payload payload;

ConstantBuffer<WorldTransform> worldTransform  : register(b0);
ConstantBuffer<MeshletInfo> meshletInfo  : register(b4);
ConstantBuffer<Frustum> frustum  : register(b5);
ConstantBuffer<WaveParam> waveParam: register(b7);

StructuredBuffer<CullData> cullData : register(t5);
StructuredBuffer<WaveData> waveData :register(t6);
StructuredBuffer<uint32_t> waveIndexData :register(t7);


bool IsVisible(Sphere sphere, WorldTransform worldTransform) {

	for (uint32_t i = 0; i < 6; i++) {
		float distance = dot(frustum.plane[i].normal, sphere.position) - frustum.plane[i].distance;
		if (distance > sphere.radius) {
			return false;
		}
	}
	return true;
}

bool IsHit(Sphere sphere, float32_t3 pos,float32_t radius) {
	float distance = length(sphere.position - pos);
	float sum = sphere.radius + radius;

	return distance <= sum;
}

[numthreads(32, 1, 1)]
void main(uint32_t dtid : SV_DispatchThreadID)
{
	bool isVisible = false;
	bool isHitWave = false;

	Sphere cullSphere;
	cullSphere.position =  Multiply(worldTransform.world, cullData[dtid].sphere.position);
	cullSphere.radius =  cullData[dtid].sphere.radius * worldTransform.scale;
	
	if (dtid < meshletInfo.meshletNum) {
		isVisible = IsVisible(cullSphere, worldTransform);
	}

	if (isVisible) {
		
		uint32_t index;

		isHitWave = true;

		for (int i = 0; i < waveIndexData[0];i++) {
			WaveData wave = waveData[waveIndexData[i + 1]];
			if (IsHit(cullSphere, wave.position, waveParam.radius)) {
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