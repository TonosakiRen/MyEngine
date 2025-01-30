struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

struct T {
	int time;
	float32_t intensity;
	float32_t speed;
	float32_t scale;
};
ConstantBuffer<T> value  : register(b0);


float32_t BlockNoise(float32_t2 num) {
	num = floor(num);
	return frac(sin(dot(num, float32_t2(12.9898, 78.233))) * 43758.5453);
};

float32_t Random(float32_t2 num) {
	return frac(sin(dot(num, float32_t2(12.9898, 78.233))) * 43758.5453);
};

float32_t NoiseRandom(float32_t2 num){
	return -1.0f + 2.0f * BlockNoise(num);
}

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float32_t4 main(VSOutput input) : SV_TARGET{
	PixelShaderOutput output;


	float32_t2 gv = input.uv;
	float noise = BlockNoise(float32_t2(input.uv.y * value.scale + value.time,input.uv.y * value.scale + value.time));
	
	gv.x += (-1.0f + 2.0f * noise) * 0.01f * sin(value.time * value.speed) * Random(float32_t2(value.time, input.uv.y));

	output.color.r = tex.Sample(smp, gv + float32_t2(0.003f,0)).r;
	output.color.g = tex.Sample(smp, gv).g;
	output.color.b = tex.Sample(smp, gv - float32_t2(0.006f,0)).b;
	output.color.a = 1.0f;

	return output.color;
}