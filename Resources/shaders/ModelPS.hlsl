#include "Common.hlsli"
Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);
ConstantBuffer<Material> gMaterial  : register(b2);

struct VSOutput {
	float32_t4 pos : SV_POSITION; // システム用頂点座標
	float32_t3 normal : NORMAL;     // 法線ベクトル
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
	output.material = gMaterial.enableLighting;

	// マテリアル
	float32_t4 tranformedUV = mul(float32_t4(input.uv, 0.0f, 1.0f), gMaterial.uvTransfrom);
	float32_t4 texColor = tex.Sample(smp, tranformedUV.xy);
	output.color = gMaterial.materialColor * texColor;

	output.normal.xyz = (normal.xyz + 1.0f) * 0.5f;
	output.normal.w = 1.0f;

	

	return output;
}