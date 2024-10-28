#include "Common.hlsli"
#include "Lighting.hlsli"
#define MY_TEXTURE2D_SPACE space1
Texture2D<float4> Texture2DTable[]  : register(t0, MY_TEXTURE2D_SPACE);
TextureCube<float32_t4> tex : register(t0);
SamplerState smp : register(s0);

RWStructuredBuffer<uint32_t> gTBRPointLightIndex  : register(u1);
RWStructuredBuffer<uint32_t> gTBRSpotLightIndex  : register(u2);
RWStructuredBuffer<uint32_t> gTBRShadowSpotLightIndex  : register(u3);
ConstantBuffer<ViewProjection> gViewProjection  : register(b1);
StructuredBuffer<DirectionalLight> gDirectionLights  : register(t6);
StructuredBuffer<PointLight> gPointLights  : register(t7);
StructuredBuffer<AreaLight> gAreaLights  : register(t8);
StructuredBuffer<SpotLight> gSpotLights  : register(t9);
StructuredBuffer<ShadowSpotLight> gShadowSpotLights  : register(t10);
ConstantBuffer<TileNum> tileNum : register(b5);
ConstantBuffer<Color> topColor : register(b6);
ConstantBuffer<Color> bottomColor : register(b7);
ConstantBuffer<Material> gMaterial  : register(b2);
RWStructuredBuffer<TBRInformation> gTBRInformation  : register(u0);


struct MSOutput {
	float32_t4 pos : SV_POSITION;
	float32_t3 normal : NORMAL;
	float32_t2 uv : TEXCOORD;
	uint32_t meshletIndex : CUSTOM_MESHLET_ID;
	float32_t3  worldPosition : POSITION0;
	float32_t  depth : TEXCOORD1;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
	float32_t4 normal : SV_TARGET1;
};

float32_t RandomRange(float32_t2 num, float32_t minValue, float32_t maxValue) {

	float32_t rand = frac(sin(dot(num, float32_t2(12.9898, 78.233))) * 43758.5453);

	return lerp(minValue, maxValue, rand);
}

PixelShaderOutput main(MSOutput input) {

	PixelShaderOutput output;

	float32_t3 normal = normalize(input.normal);

	float32_t3 cameraToPosition = normalize(input.worldPosition - gViewProjection.viewPosition);
	float32_t3 reflectedVector = normalize(reflect(cameraToPosition,normal));
	float32_t4 environmentColor = tex.Sample(smp,reflectedVector);
	
	float32_t gradationT = (reflectedVector.y + 1.0f) * 0.5f;
	float32_t4 hsva = lerp(bottomColor.t,topColor.t,gradationT);
	float32_t4 skyColor = {0.0f,0.0f,0.0f,1.0f};
	skyColor.xyz = HSVToRGB(hsva.xyz);
	skyColor += environmentColor;

	// マテリアル
	float32_t4 tranformedUV = mul(float32_t4(input.uv, 0.0f, 1.0f), gMaterial.uvTransfrom);
	float32_t4 color = gMaterial.materialColor * skyColor;	

	if (gMaterial.enableLighting) {

		float32_t3 lighting = { 0.0f,0.0f,0.0f };

		float32_t3 shading = { 1.0f,1.0f,1.0f };

		//tile
		uint32_t tileNumber = GetTileInformationIndex(input.pos.xy,tileNum);

		TBRInformation tileInformation = gTBRInformation[tileNumber];

		float32_t3 viewDirection = normalize(gViewProjection.viewPosition -  input.worldPosition);

		for (int i = 0; i < tileInformation.pointLightNum; i++) {

			int index = gTBRPointLightIndex[tileInformation.pointLightOffset + i];

			lighting += PointLightLighting(gPointLights[index], input.worldPosition,viewDirection,normal);
		}

		//spotLight

		for (int j = 0; j < tileInformation.spotLightNum; j++) {

			int index = gTBRSpotLightIndex[tileInformation.spotLightOffset + j];

			lighting += SpotLightLighting(gSpotLights[index], input.worldPosition,viewDirection,normal);

		}

		//shadowSpotLight

		for (int l = 0; l < tileInformation.shadowSpotLightNum; l++) {

			int index = gTBRShadowSpotLightIndex[tileInformation.shadowSpotLightOffset + l];

			lighting += ShadowSpotLightLighting(gShadowSpotLights[index], input.worldPosition,viewDirection,normal);

			//影
			float32_t4 wp = float4( input.worldPosition.xyz, 1.0f);
			float32_t4 lightViewPosition = mul(wp, gShadowSpotLights[index].viewProjection);
			float32_t2 shadowMapUV = lightViewPosition.xy / lightViewPosition.w;
			shadowMapUV *= float32_t2(0.5f, -0.5f);
			shadowMapUV += 0.5f;

			/*if (lightViewPosition.z > 0.0f) {
				float32_t zInLVP = lightViewPosition.z / lightViewPosition.w;

				if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f
					&& shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f
					) {
					float32_t zInShadowMap = Texture2DTable[gShadowSpotLights[index].descriptorIndex].Sample(smp, shadowMapUV).r;
					if (zInShadowMap != 1.0f) {
						if (zInLVP - 0.00001 > zInShadowMap) {
							shading *= 0.5f;
						}
					}
				}
			}*/

		}

		for (int k = 0; k < 1; k++) {

			lighting += DirectionalLightLighting(gDirectionLights[k], input.worldPosition,viewDirection,normal);

			//影
			/*float32_t4 wp = float4( input.worldPosition.xyz, 1.0f);
			float32_t4 lightViewPosition = mul(wp, gDirectionLights[k].viewProjection);
			float32_t2 shadowMapUV = lightViewPosition.xy / lightViewPosition.w;
			shadowMapUV *= float32_t2(0.5f, -0.5f);
			shadowMapUV += 0.5f;

			float32_t zInLVP = lightViewPosition.z / lightViewPosition.w;

			if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f
				&& shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f
				) {
				float32_t zInShadowMap = Texture2DTable[gDirectionLights[k].descriptorIndex].Sample(smp, shadowMapUV).r;
				if (zInLVP - 0.00001 > zInShadowMap) {
					shading *= 0.5f;
				}
			}*/

		}

		for (int m = 0; m < 1; m++) {

			lighting += AreaLightLighting(gAreaLights[m], input.worldPosition,viewDirection,normal);
		}
		//アンビエント

		lighting *= shading;

		lighting += ambient;

		color.xyz *= lighting;

	}

	output.color = color;

	//float32_t a = RandomRange(float32_t2(input.meshletIndex, input.meshletIndex),0.0f,1.0f);
	//float32_t b = RandomRange(float32_t2(input.meshletIndex, 0.0f), 0.0f, 1.0f);
	//float32_t c = RandomRange(float32_t2(0.0f, input.meshletIndex), 0.0f, 1.0f);
	//output.color = float32_t4(a, b, c, 1.0f);

	output.normal.xyz = (normal.xyz + 1.0f) * 0.5f;
	output.normal.w = 1.0f;

	return output;
}