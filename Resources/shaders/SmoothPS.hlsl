struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

struct T {
	float32_t value;
};
ConstantBuffer<T> t  : register(b0);

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

static const float32_t2 kIndex3x3[3][3] = {
	{{-1.0f,-1.0f},{0.0f,-1.0f},{1.0f,-1.0f}},
	{{-1.0f,0.0f},{0.0f,0.0f},{1.0f,0.0f}},
	{{-1.0f,1.0f},{0.0f,1.0f},{1.0f,1.0f}},
};

static const float32_t kKernel3x3[3][3] = {
	{1.0f / 9.0f , 1.0f / 9.0f,1.0f / 9.0f},
	{1.0f / 9.0f , 1.0f / 9.0f,1.0f / 9.0f},
	{1.0f / 9.0f , 1.0f / 9.0f,1.0f / 9.0f},
};

static const float32_t2 kIndex5x5[5][5] = {
	{{-2.0f,-2.0f},{-1.0f,-2.0f},{0.0f,-2.0f},{1.0f,-2.0f},{2.0f,-2.0f}},
	{{-2.0f,-1.0f},{-1.0f,-1.0f},{0.0f,-1.0f},{1.0f,-1.0f},{2.0f,-1.0f}},
	{{-2.0f,0.0f},{-1.0f,0.0f},{0.0f,0.0f},{1.0f,0.0f},{2.0f,0.0f}},
	{{-2.0f,1.0f},{-1.0f,1.0f},{0.0f,-1.0f},{1.0f,-1.0f},{2.0f,-1.0f}},
	{{-2.0f,2.0f},{-1.0f,2.0f},{0.0f,-2.0f},{1.0f,-2.0f},{2.0f,-2.0f}},
};

static const float32_t kKernel5x5[5][5] = {
	{1.0f / 25.0f , 1.0f / 25.0f,1.0f / 25.0f, 1.0f / 25.0f,1.0f / 25.0f},
	{1.0f / 25.0f , 1.0f / 25.0f,1.0f / 25.0f, 1.0f / 25.0f,1.0f / 25.0f},
	{1.0f / 25.0f , 1.0f / 25.0f,1.0f / 25.0f, 1.0f / 25.0f,1.0f / 25.0f},
	{1.0f / 25.0f , 1.0f / 25.0f,1.0f / 25.0f, 1.0f / 25.0f,1.0f / 25.0f},
	{1.0f / 25.0f , 1.0f / 25.0f,1.0f / 25.0f, 1.0f / 25.0f,1.0f / 25.0f},
};

float32_t4 main(VSOutput input) : SV_TARGET{
	uint32_t width,height;
	tex.GetDimensions(width, height);
	float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));

	PixelShaderOutput output;
	output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
	output.color.a = 1.0f;
	for (int32_t x = 0; x < 5; ++x) {
		for (int32_t y = 0; y < 5; ++y) {
			float32_t2 texcoord = input.uv + (kIndex5x5[x][y] * uvStepSize);
			float32_t3 fetchColor = tex.Sample(smp, texcoord).rgb;
			output.color.rgb += fetchColor * kKernel5x5[x][y];
		}
	}

	return output.color;
}