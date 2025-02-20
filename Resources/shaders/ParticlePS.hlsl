#include "Common.hlsli"
Texture2D<float4> tex : register(t1);
SamplerState smp : register(s0);
ConstantBuffer<Material> gMaterial  : register(b1);

struct VSOutput {
	float32_t4 pos : SV_POSITION;
	float32_t3 normal : NORMAL;
	float32_t2 uv : TEXCOORD;
	float32_t3 worldPosition : POSITION0;
	float32_t depth : TEXCOORD1;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
	float32_t4 normal : SV_TARGET1;
	float32_t material : SV_TARGET2;
};

PixelShaderOutput main(VSOutput input) {
	PixelShaderOutput output;

	float32_t3 normal = normalize(input.normal);
	output.normal.xyz = (normal.xyz + 1.0f) * 0.5f;
	output.normal.w = gMaterial.enableLighting;
	output.material = gMaterial.enableLighting;

	float32_t4 tranformedUV = mul(float32_t4(input.uv, 0.0f, 1.0f), gMaterial.uvTransfrom);
	float32_t4 texcolor = tex.Sample(smp, tranformedUV.xy);
	output.color = texcolor * gMaterial.materialColor;
	if (output.color.a == 0.0) {
		discard;
	}
	return output;
}