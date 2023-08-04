
Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

struct DirectionLight {
	float32_t4 color;     //ライトの色
	float32_t3 direction; //ライト向き
	float32_t intensity;   //輝度
};
ConstantBuffer<DirectionLight> gDirectionLight  : register(b2);

struct Material {
	float32_t4 materialcolor; //Materialの色
	float32_t4x4 uvTransfrom;//uvtransform
};
ConstantBuffer<Material> gMaterial  : register(b3);

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput {
	float32_t4 svpos : SV_POSITION; // システム用頂点座標
	float32_t3 normal : NORMAL;     // 法線ベクトル
	float32_t2 uv : TEXCOORD;       // uv値
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VSOutput input){

	PixelShaderOutput output;
	output.color = float32_t4(1.0f, 1.0f, 1.0f, 1.0f);
  
  float32_t4 tranformedUV = mul(float32_t4(input.uv, 0.0f, 1.0f), gMaterial.uvTransfrom);
  float32_t4 texcolor = tex.Sample(smp, tranformedUV.xy);
  float32_t NdotL = dot(normalize(input.normal), -normalize(gDirectionLight.direction));
  float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
  output.color.xyz = gMaterial.materialcolor.xyz * texcolor.xyz * gDirectionLight.color.xyz * cos * gDirectionLight.intensity;
  return output;
}