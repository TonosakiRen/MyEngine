#include "Common.hlsli"
Texture2D<float> tex : register(t0);
SamplerState smp : register(s0);

struct Param {
	float32_t4 color;
};
ConstantBuffer<Param> color  : register(b2);

struct Time {
	uint32_t t;
};
ConstantBuffer<Time> time  : register(b3);


struct VSOutput {
	float32_t4 pos : SV_POSITION; // システム用頂点座標
	float32_t3 normal : NORMAL;     // 法線ベクトル
	float32_t2 uv : TEXCOORD;       // uv値
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
	float32_t4 normal : SV_TARGET1;
	float32_t enableLighting : SV_TARGET2;
};

float32_t3 HSVToRGB(in float32_t3 hsv) {
	float32_t4 k = float32_t4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f);
	float32_t3 p = abs(frac(hsv.xxx + k.xyz) * 6.0f - k.www);
	return hsv.z * lerp(k.xxx, clamp(p - k.xxx, 0.0f, 1.0f), hsv.y);
}

PixelShaderOutput main(VSOutput input) {

	PixelShaderOutput output;

	float32_t3 normal = normalize(input.normal);
	output.enableLighting = 1.0f;
	// マテリアル
	output.color = color.color;

	float32_t2 uv = input.uv;
	uv.y += time.t / 1000.0f;

	float32_t textureColor = tex.Sample(smp, uv);
	output.color.xyz += textureColor * 1.0f;

	output.normal.xyz = (normal.xyz + 1.0f) * 0.5f;
	output.normal.w = 1.0f;

	return output;
}