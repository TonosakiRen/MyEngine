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

float32_t4 main(VSOutput input) : SV_TARGET{
	PixelShaderOutput output;
	output.color = tex.Sample(smp, input.uv);
	
	float32_t2 texSize;
	tex.GetDimensions(texSize.x, texSize.y);

	float32_t2 texPos = input.uv * texSize;

	float32_t2 center = texSize / 2.0f;

	float32_t maxRadius = length(center);

	float32_t distance = length(texPos - center);

	float32_t radius = lerp(0.0f, maxRadius,t.value);


	if (radius - distance < 0.0f) {
		return output.color;
	}

	float32_t value = dot(output.color.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
	output.color.rgb = float32_t3(value, value, value);

	return output.color;
}