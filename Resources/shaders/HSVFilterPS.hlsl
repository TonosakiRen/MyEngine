struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

struct T {
	float32_t3 hsv;
};
ConstantBuffer<T> t  : register(b0);

float32_t WrapValue(float32_t value, float32_t minRange, float32_t maxRange) {
	float32_t range = maxRange - minRange;
	float32_t modValue = fmod(value - minRange, range);
	if (modValue < 0) {
		modValue += range;
	}
	return minRange + modValue;
}

float32_t3 HSVToRGB(in float32_t3 hsv) {
	float32_t4 k = float32_t4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f);
	float32_t3 p = abs(frac(hsv.xxx + k.xyz) * 6.0f - k.www);
	return hsv.z * lerp(k.xxx, clamp(p - k.xxx, 0.0f, 1.0f), hsv.y);
}


float32_t3 RGBToHSV(in float32_t3 rgb) {
	float32_t4 k = float32_t4(0.0f,-1.0f/3.0f,2.0f/3.0f,-1.0f);
	float32_t4 p = lerp(float32_t4(rgb.b, rgb.g, k.w, k.z), float32_t4(rgb.g, rgb.b, k.x, k.y), step(rgb.b, rgb.g));
	float32_t4 q = lerp(float32_t4(p.x, p.y, p.w, rgb.r), float32_t4(rgb.r, p.y, p.z, p.x), step(p.x, rgb.r));

	float32_t d = q.x - min(q.w, q.y);
	float32_t e = 1.0e-10;
	return float32_t3(abs(q.z + (q.w - q.y) / (6.0f * d + e)), d / (q.x + e), q.x);
}

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float32_t4 main(VSOutput input) : SV_TARGET{
	PixelShaderOutput output;
	output.color = tex.Sample(smp, input.uv);
	float32_t3 hsv = RGBToHSV(output.color.rgb);

	hsv += t.hsv;

	hsv.x = WrapValue(hsv.x, 0.0f, 1.0f);
	hsv.y = saturate(hsv.y);
	hsv.z = saturate(hsv.z);

	float32_t3 rgb = HSVToRGB(hsv);
	
	output.color.rgb = rgb;

	return output.color;
}