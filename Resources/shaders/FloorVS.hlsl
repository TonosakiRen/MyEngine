#include "Common.hlsli"
ConstantBuffer<WorldTransform> gWorldTransform  : register(b0);
ConstantBuffer<ViewProjection> gViewProjection  : register(b1);

struct VSOutput {
	float32_t4 pos : SV_POSITION;
	float32_t3 normal : NORMAL;
	float32_t2 uv : TEXCOORD;
};

VSOutput main(float32_t4 pos : POSITION, float32_t3 normal : NORMAL, float32_t2 uv : TEXCOORD) {
	VSOutput output; // ピクセルシェーダーに渡す値
	output.pos = mul(pos, mul(gWorldTransform.world, gViewProjection.viewProjection));
	output.normal = normal;
	output.uv = uv;
	return output;
}