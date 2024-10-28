#include "Common.hlsli"
#include "Lighting.hlsli"
#define MY_TEXTURE2D_SPACE space1
Texture2D<float4> Texture2DTable[]  : register(t0, MY_TEXTURE2D_SPACE);
Texture2D<float4> tex : register(t1);
SamplerState smp : register(s0);

RWStructuredBuffer<uint32_t> gTBRPointLightIndex  : register(u1);
RWStructuredBuffer<uint32_t> gTBRSpotLightIndex  : register(u2);
RWStructuredBuffer<uint32_t> gTBRShadowSpotLightIndex  : register(u3);
ConstantBuffer<ViewProjection> gViewProjection  : register(b0);
StructuredBuffer<DirectionalLight> gDirectionLights  : register(t2);
StructuredBuffer<PointLight> gPointLights  : register(t3);
StructuredBuffer<AreaLight> gAreaLights  : register(t4);
StructuredBuffer<SpotLight> gSpotLights  : register(t5);
StructuredBuffer<ShadowSpotLight> gShadowSpotLights  : register(t6);
ConstantBuffer<TileNum> tileNum : register(b2);
ConstantBuffer<Material> gMaterial  : register(b1);
RWStructuredBuffer<TBRInformation> gTBRInformation  : register(u0);

struct VSOutput {
	float32_t4 pos : SV_POSITION;
	float32_t3 normal : NORMAL;
	float32_t2 uv : TEXCOORD;
	float32_t3 worldPosition : POSITION0;
	float32_t depth : TEXCOORD1;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
	float32_t4 normal : SV_TARGET1;
};

PixelShaderOutput main(VSOutput input) {
	PixelShaderOutput output;

	float32_t3 normal = normalize(input.normal);
	output.normal.xyz = (normal.xyz + 1.0f) * 0.5f;
	output.normal.w = 1.0f;

	float32_t4 tranformedUV = mul(float32_t4(input.uv, 0.0f, 1.0f), gMaterial.uvTransfrom);
	float32_t4 texColor = tex.Sample(smp, tranformedUV.xy);
	float32_t4 color = gMaterial.materialColor * texColor;
	if (output.color.a == 0.0) {
		discard;
	}

	if (gMaterial.enableLighting) {

		float32_t depth = input.depth;

		float32_t3 worldPos = input.worldPosition;

		float32_t3 lighting = { 0.0f,0.0f,0.0f };

		float32_t3 shading = { 1.0f,1.0f,1.0f };

		//tile
		uint32_t tileNumber = GetTileInformationIndex(input.pos.xy,tileNum);

		TBRInformation tileInformation = gTBRInformation[tileNumber];

		float32_t3 viewDirection = normalize(gViewProjection.viewPosition - worldPos);

		for (int i = 0; i < tileInformation.pointLightNum; i++) {

			int index = gTBRPointLightIndex[tileInformation.pointLightOffset + i];

			lighting += PointLightLighting(gPointLights[index],worldPos,viewDirection,normal);
		}

		//spotLight

		for (int j = 0; j < tileInformation.spotLightNum; j++) {

			int index = gTBRSpotLightIndex[tileInformation.spotLightOffset + j];

			lighting += SpotLightLighting(gSpotLights[index],worldPos,viewDirection,normal);

		}

		//shadowSpotLight

		for (int l = 0; l < tileInformation.shadowSpotLightNum; l++) {

			int index = gTBRShadowSpotLightIndex[tileInformation.shadowSpotLightOffset + l];

			lighting += ShadowSpotLightLighting(gShadowSpotLights[index],worldPos,viewDirection,normal);

			//影
			float32_t4 wp = float4(worldPos.xyz, 1.0f);
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

			lighting += DirectionalLightLighting(gDirectionLights[k],worldPos,viewDirection,normal);

			//影
			/*float32_t4 wp = float4(worldPos.xyz, 1.0f);
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

			lighting += AreaLightLighting(gAreaLights[m],worldPos,viewDirection,normal);
		}

		//アンビエント

		lighting *= shading;

		lighting += ambient;

		color.xyz *= lighting;

	}

	output.color = color;
	return output;
}