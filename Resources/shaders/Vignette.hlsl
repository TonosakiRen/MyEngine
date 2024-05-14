struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

struct T {
	float32_t scale;
	float32_t t;
};
ConstantBuffer<T> value  : register(b0);

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float32_t4 main(VSOutput input) : SV_TARGET{
	PixelShaderOutput output;
	output.color = tex.Sample(smp, input.uv);
	
	// 周囲を0に、中心になるほど明るくなるように計算で調整
	float32_t2 correct = input.uv * (1.0f - input.uv.yx);
	// correctだけで計算すると中心の最大値が0.0625で暗すぎるのでScaleで調整。
	float vignette = correct.x * correct.y * value.scale;
	//とりあえず0.8上でそれっぽく
	vignette = saturate(pow(vignette, value.t));
	//係数として乗算
	output.color.rgb *= vignette;


	return output.color;
}