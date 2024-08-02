#include "Common.hlsli"
ConstantBuffer<ViewProjection> gViewProjection  : register(b0);

struct VSOutput {
	float32_t4 pos : SV_POSITION;
};

VSOutput main(float32_t4 pos : POSITION) {
	VSOutput output; // ピクセルシェーダーに渡す値
	output.pos = mul(pos, gViewProjection.viewProjection);
	return output;
}