struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

float32_t Random(float32_t2 num) {
	num = floor(num);
	return frac(sin(dot(num, float32_t2(12.9898, 78.233))) * 43758.5453);
};

float32_t RandomRange(float32_t2 num , float32_t minValue , float32_t maxValue) {
	
	float32_t rand = frac(sin(dot(num, float32_t2(12.9898, 78.233))) * 43758.5453);

	return lerp(minValue,maxValue, rand);
};

SamplerState smp : register(s0);

float32_t4 main(VSOutput input) : SV_TARGET{
	PixelShaderOutput output;

	float32_t2 uv = input.uv * 200.0f;
	float value = Random(uv);
	float32_t4 color;
	if(value < 0.3333f){
		color = float4(0.0f, 1.0f, 1.0f, 1.0f);
	}else if(value < 0.6666f){
		color = float4(1.0f, 0.0f, 1.0f, 1.0f); 
	}else{
		color = float4(1.0f, 1.0f, 0.0f, 1.0f);
	}

	output.color = color;

	return output.color;
}