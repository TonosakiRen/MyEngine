
struct cbuff0 {
	float32_t4  color; // 色(RGBA)
	float32_t4x4 mat;   // ３Ｄ変換行列
};
ConstantBuffer<cbuff0> gCbuff0  : register(b0);
struct VSOutput {
	float32_t4  svpos : SV_POSITION; // システム用頂点座標
	float32_t2 uv : TEXCOORD;       // uv値
};
VSOutput main(float32_t4 pos : POSITION, float32_t2 uv : TEXCOORD) {
	VSOutput output; // ピクセルシェーダーに渡す値
	output.svpos = mul(pos, gCbuff0.mat);
	output.uv = uv;
	return output;
}