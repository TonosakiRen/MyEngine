
struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

struct ViewProjection {
	float32_t4x4 view;
	float32_t4x4 projection;
	float32_t3 viewPosition;
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b0);

struct DirectionLight {
	float32_t4 color;     //ライトの色
	float32_t3 direction; //ライト向き
	float32_t intensity;   //輝度
};
ConstantBuffer<DirectionLight> gDirectionLight  : register(b1);

Texture2D<float4> colorTex : register(t0);
Texture2D<float4> normalTex : register(t1);
Texture2D<float4> depthTex : register(t2);

SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	PixelShaderOutput output;
	
	float32_t3 color = colorTex.Sample(smp, input.uv).xyz;
	float32_t3 normal = normalTex.Sample(smp, input.uv).xyz;
	normal = normal * 2.0f - 1.0f;
	float32_t depth = colorTex.Sample(smp, input.uv).x;

	//陰
	float32_t NdotL = dot(normal, -normalize(gDirectionLight.direction));
	float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 diffuse = gDirectionLight.color.xyz * cos * gDirectionLight.intensity;

	//反射
	/*float32_t3 viewDirection = normalize(gViewProjection.viewPosition - input.worldPosition);

	float32_t3 reflectVec = reflect(gDirectionLight.direction, normal);
	float32_t3 speculerColor = float32_t3(1.0f, 1.0f, 1.0f);
	float32_t specluerPower = 100.0f;
	float32_t3 specluer = speculerColor * pow(saturate(dot(reflectVec, viewDirection)), specluerPower);*/

	//アンビエント
	float32_t3 ambient = float32_t3(0.3f, 0.3f, 0.3f);


	color.xyz *= (diffuse + ambient);

	output.color.xyz = color;

	return output.color;
}