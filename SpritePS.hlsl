

struct cbuff0 {
	float32_t4  color; // 色(RGBA)
	float32_t4x4 mat;   // ３Ｄ変換行列
};
ConstantBuffer<cbuff0> gCbuff0  : register(b0);
struct VSOutput {
	float32_t4  svpos : SV_POSITION; // システム用頂点座標
	float32_t2  uv : TEXCOORD;       // uv値
};

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float32_t4 main(VSOutput input) : SV_TARGET{ return tex.Sample(smp, input.uv) * gCbuff0.color; }