#include "Common.hlsli"
#include "Lighting.hlsli"
#define MY_TEXTURE2D_SPACE space1

struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

struct Param {
	uint32_t2 param;
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b0);
StructuredBuffer<DirectionalLight> gDirectionLights  : register(t4);
StructuredBuffer<PointLight> gPointLights  : register(t5);
StructuredBuffer<AreaLight> gAreaLights  : register(t6);
StructuredBuffer<SpotLight> gSpotLights  : register(t7);
StructuredBuffer<ShadowSpotLight> gShadowSpotLights  : register(t8);
ConstantBuffer<LightNum> lightNum : register(b1);
RWStructuredBuffer<TBRInformation> gTBRInformation  : register(u0);

RWStructuredBuffer<uint32_t> gTBRPointLightIndex  : register(u1);
RWStructuredBuffer<uint32_t> gTBRSpotLightIndex  : register(u2);
RWStructuredBuffer<uint32_t> gTBRShadowSpotLightIndex  : register(u3);

ConstantBuffer<Param> tileNum : register(b2);


float3 GetWorldPosition(in float2 texcoord, in float depth, in float4x4 viewProjectionInverseMatrix) {
	// xは0~1から-1~1, yは0~1から1~-1に上下反転
	float2 xy = texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
	float4 position = float4(xy, depth, 1.0f);
	position = mul(position, viewProjectionInverseMatrix);
	position.xyz /= position.w;
	return position.xyz;
}


Texture2D<float4> colorTex : register(t0);
Texture2D<float4> normalTex : register(t1);
Texture2D<float4> materialTex : register(t2);
Texture2D<float4> depthTex : register(t3);

Texture2D<float4> Texture2DTable[]  : register(t0, MY_TEXTURE2D_SPACE);

SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	PixelShaderOutput output;

	float32_t3 color = colorTex.Sample(smp, input.uv).xyz;

	float32_t4 normalSample = normalTex.Sample(smp, input.uv);
	float32_t3 normal = normalSample.xyz * 2.0f - 1.0f;
	float32_t enableLighting = normalSample.w;

	if (enableLighting) {

		float32_t depth = depthTex.Sample(smp, input.uv).x;

		float32_t3 worldPos = GetWorldPosition(input.uv, depth, gViewProjection.inverseViewProjection);

		float32_t3 lighting = { 0.0f,0.0f,0.0f };

		float32_t3 shading = { 1.0f,1.0f,1.0f };

		//tile
		uint32_t tileNumber = GetTileInformationIndex(input.uv * texSize,tileNum.param);

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

		// フレネル
			//float32_t3 fresnelColor = float32_t3(1.0f, 0.0f, 0.0f);
			//float32_t power = 2.0f;
			//float32_t fresnel = pow((1.0f - saturate(dot(normal, viewDirection))), power);
			////output.color.xyz = lerp(1.0f - fresnel, output.color.xyz, fresnelColor);
			//output.color.xyz += fresnelColor * fresnel;

		lighting *= shading;

		lighting += ambient;

		color.xyz *= lighting;

	}

	output.color.xyz = color.xyz;
	output.color.w = 1.0f;

	return output.color;
}