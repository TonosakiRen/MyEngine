
Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

struct Material {
	float32_t4 materialColor; //Materialの色
	float32_t4x4 uvTransfrom;//uvtransform
	int32_t enableLighting; //lighitngするか
};
ConstantBuffer<Material> gMaterial  : register(b2);

struct MSOutput {
	float32_t4 pos : SV_POSITION; 
	float32_t3 normal : NORMAL;
	float32_t2 uv : TEXCOORD;
	uint32_t meshletIndex : CUSTOM_MESHLET_ID;
	float32_t3 worldPosition : POSITION0;
	float32_t depth : TEXCOORD1;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
	float32_t4 normal : SV_TARGET1;
	float32_t material : SV_TARGET2;
};

float32_t RandomRange(float32_t2 num, float32_t minValue, float32_t maxValue) {

	float32_t rand = frac(sin(dot(num, float32_t2(12.9898, 78.233))) * 43758.5453);

	return lerp(minValue, maxValue, rand);
}

PixelShaderOutput main(MSOutput input) {

	PixelShaderOutput output;

	float32_t3 normal = normalize(input.normal);
	output.material = 1.0f;

	// マテリアル
	//float32_t4 tranformedUV = mul(float32_t4(input.uv, 0.0f, 1.0f), gMaterial.uvTransfrom);
	//float32_t4 texColor = tex.Sample(smp, tranformedUV.xy);
	//output.color = gMaterial.materialColor * texColor;

	float32_t a = RandomRange(float32_t2(input.meshletIndex, input.meshletIndex),0.0f,1.0f);
	float32_t b = RandomRange(float32_t2(input.meshletIndex, 0.0f), 0.0f, 1.0f);
	float32_t c = RandomRange(float32_t2(0.0f, input.meshletIndex), 0.0f, 1.0f);
	output.color = float32_t4(a, b, c, 1.0f);

	output.normal.xyz = (normal.xyz + 1.0f) * 0.5f;
	output.normal.w = 1.0f;

	return output;
}