#include "Common.hlsli"
ConstantBuffer<WorldTransform> worldTransform  : register(b0);
ConstantBuffer<Frustum> frustum  : register(b4);
ConstantBuffer<MeshletInfo> meshletInfo  : register(b3);
StructuredBuffer<CullData> cullData : register(t5);

struct Payload {
	uint32_t meshletIndices[32];
};
groupshared Payload payload;

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