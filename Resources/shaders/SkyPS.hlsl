
TextureCube<float32_t4> tex : register(t0);
SamplerState smp : register(s0);

struct Param {
	float32_t4 color;
};
ConstantBuffer<Param> topColor  : register(b2);
ConstantBuffer<Param> bottomColor  : register(b3);


struct VSOutput {
	float32_t4 pos : SV_POSITION0;
	float32_t4 localPos : TEXCOORD0;
	float32_t3 uv : TEXCOORD1;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
	float32_t4 normal : SV_TARGET1;
};

float32_t Random(float32_t2 num) {
	return frac(sin(dot(num, float32_t2(12.9898, 78.233))) * 43758.5453);
}

float32_t RandomRange(float32_t2 num , float32_t minValue , float32_t maxValue) {
	
	float32_t rand = frac(sin(dot(num, float32_t2(12.9898, 78.233))) * 43758.5453);

	return lerp(minValue,maxValue, rand);
}

float32_t3 HSVToRGB(in float32_t3 hsv) {
	float32_t4 k = float32_t4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f);
	float32_t3 p = abs(frac(hsv.xxx + k.xyz) * 6.0f - k.www);
	return hsv.z * lerp(k.xxx, clamp(p - k.xxx, 0.0f, 1.0f), hsv.y);
}

PixelShaderOutput main(VSOutput input) {

	PixelShaderOutput output;


	output.normal = float32_t4(0.0f, 0.0f, 0.0f, 0.0f); 

	output.color = float32_t4(0.0f, 0.0f, 0.0f, 1.0f);
	
	float32_t t = (input.localPos.y + 1.0f) * 0.5f;

	float32_t4 hsva = lerp(topColor.color, bottomColor.color, t);
	output.color.xyz = HSVToRGB(hsva.xyz);

	float32_t4 textureColor = tex.Sample(smp, input.uv);
	output.color += textureColor;

	/*output.enableLighting.x = 0.0f;

	output.color = float32_t4(0.0f, 0.0f, 0.0f, 1.0f);

	if (input.normal.y <= -0.9f) {
		output.color.xyz = HSVToRGB(topColor.color.xyz);
	}
	else if (input.normal.y >= 0.9f) {
		output.color.xyz = HSVToRGB(bottomColor.color.xyz);
	}
	else {
		float32_t4 hsva = lerp(topColor.color, bottomColor.color, input.uv.y);

		output.color.xyz = HSVToRGB(hsva.xyz);
	}

	output.color += tex.Sample(smp, input.uv * 3.5f);

	output.normal = float32_t4(0.0f, 0.0f, 0.0f, 1.0f);*/

	return output;
}