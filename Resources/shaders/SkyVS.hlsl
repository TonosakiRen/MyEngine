#include "Common.hlsli"
ConstantBuffer<WorldTransform> gWorldTransform  : register(b0);
ConstantBuffer<ViewProjection> gViewProjection  : register(b1);

struct VSOutput {
	float32_t4 pos : SV_POSITION0;
	float32_t4 localPos : TEXCOORD0;
	float32_t3 uv : TEXCOORD1;
};

VSOutput main(float32_t4 pos : POSITION) {
	VSOutput output; // ピクセルシェーダーに渡す値
	output.pos = mul(float32_t4(pos.xyz, 1.0f), mul(gWorldTransform.world, gViewProjection.viewProjection)).xyww;
	output.localPos = pos;
	output.uv = pos.xyz;
	return output;
}