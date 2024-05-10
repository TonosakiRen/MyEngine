
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

struct ViewProjection {
	float32_t4x4 viewProjection;
	float32_t4x4 inverseViewProjection;
	float32_t3 viewPosition;
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b0);

struct DirectionLight {
	float32_t4 color;
	float32_t3 direction;
	float32_t intensity;
	float32_t4x4 viewProjection;
	uint32_t descriptorIndex;
};
StructuredBuffer<DirectionLight> gDirectionLights  : register(t4);

struct PointLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t radius;
	float32_t decay;
	float32_t isActive;
};
StructuredBuffer<PointLight> gPointLights  : register(t5);

struct AreaLight {
	float32_t4 color;
	float32_t3 origin;
	float32_t3 diff;
	float32_t intensity;
	float32_t range;
	float32_t decay;
	float32_t isActive;
};
StructuredBuffer<AreaLight> gAreaLights  : register(t6);

struct SpotLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t3 direction;
	float32_t distance;
	float32_t decay;
	float32_t cosAngle;
	float32_t isActive;
};
StructuredBuffer<SpotLight> gSpotLights  : register(t7);

struct ShadowSpotLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t3 direction;
	float32_t distance;
	float32_t decay;
	float32_t cosAngle;
	float32_t isActive;
	uint32_t descriptorIndex;
	float32_t4x4 viewProjection;
};
StructuredBuffer<ShadowSpotLight> gShadowSpotLights  : register(t8);

struct LightNum {
	uint32_t  directionalLight;
	uint32_t  pointLight;
	uint32_t  areaLight;
	uint32_t  spotLight;
	uint32_t  shadowSpotLight;
};

ConstantBuffer<LightNum> lightNum : register(b1);

struct TBRInformation {
	uint32_t pointLightNum;
	uint32_t spotLightNum;
	uint32_t shadowSpotLightNum;
};
StructuredBuffer<TBRInformation> gTBRInformation  : register(t9);

StructuredBuffer<uint32_t> gTBRPointLightIndex  : register(t10);
StructuredBuffer<uint32_t> gTBRSpotLightIndex  : register(t11);
StructuredBuffer<uint32_t> gTBRShadowSpotLightIndex  : register(t12);

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

	float32_t enableLighting = materialTex.Sample(smp, input.uv).x;

	if (enableLighting) {

		float32_t3 normal = normalTex.Sample(smp, input.uv).xyz;
		normal = normal * 2.0f - 1.0f;
		float32_t depth = depthTex.Sample(smp, input.uv).x;

		float32_t3 worldPos = GetWorldPosition(input.uv, depth, gViewProjection.inverseViewProjection);

		float32_t3 lighting = { 0.0f,0.0f,0.0f };

		float32_t3 shading = { 1.0f,1.0f,1.0f };

		//tile
		float32_t2 texSize;
		colorTex.GetDimensions(texSize.x, texSize.y);

		float32_t tileWidth = texSize.x / tileNum.param.x;

		float32_t tileHeight = texSize.y / tileNum.param.y;

		float32_t2 pixelPos = input.uv * texSize;

		uint32_t2 tile = uint32_t2(pixelPos.x / tileWidth, pixelPos.y / tileHeight);

		uint32_t tileNumber = tile.y * tileNum.param.x + tile.x;

		TBRInformation tileInformation = gTBRInformation[tileNumber];

		uint32_t startPointLightIndex = lightNum.pointLight * tileNumber;

		float32_t3 viewDirection = normalize(gViewProjection.viewPosition - worldPos);

		for (int i = 0; i < tileInformation.pointLightNum; i++) {

			int index = gTBRPointLightIndex[startPointLightIndex + i];

			float32_t3 pointLightDirection = worldPos - gPointLights[index].position;
			float32_t distance = length(pointLightDirection);
			pointLightDirection = pointLightDirection / distance;
			float32_t factor = pow(saturate(-distance / gPointLights[index].radius + 1.0), gPointLights[index].decay);

			//pointLightDiffuse
			float32_t NdotL = dot(normal, -pointLightDirection);
			float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
			float32_t3 pointLightdiffuse = gPointLights[index].color.xyz * cos * gPointLights[index].intensity * factor;

			//pointLightSpecular
			float32_t3 reflectVec = reflect(pointLightDirection, normal);
			float32_t specluerPower = 10.0f;
			float32_t RdotE = dot(reflectVec, viewDirection);
			float32_t specularPow = pow(saturate(RdotE), specluerPower);
			float32_t3 pointLightSpecluer = gPointLights[index].color.xyz * gPointLights[index].intensity * specularPow * factor;

			lighting += (pointLightdiffuse + pointLightSpecluer);
		}

		//spotLight

		uint32_t startSpotLightIndex = lightNum.spotLight * tileNumber;

		for (int j = 0; j < tileInformation.spotLightNum; j++) {

			int index = gTBRSpotLightIndex[startSpotLightIndex + j];

			float32_t3 spotLightDirectionOnSurface = normalize(worldPos - gSpotLights[index].position);
			float32_t distance = length(gSpotLights[index].position - worldPos);
			float32_t factor = pow(saturate(-distance / gSpotLights[index].distance + 1.0), gSpotLights[index].decay);
			float32_t cosAngle = dot(spotLightDirectionOnSurface, gSpotLights[index].direction);
			float32_t falloffFactor = saturate((cosAngle - gSpotLights[index].cosAngle) / (1.0f - gSpotLights[index].cosAngle));


			//spotLightDiffuse
			float32_t NdotL = dot(normal, -spotLightDirectionOnSurface);
			float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
			float32_t3 spotLightdiffuse = gSpotLights[index].color.xyz * cos * gSpotLights[index].intensity * factor * falloffFactor;


			//spotLightSpecular

			float32_t3 reflectVec = reflect(spotLightDirectionOnSurface, normal);
			float32_t specluerPower = 10.0f;
			float32_t RdotE = dot(reflectVec, viewDirection);
			float32_t specularPow = pow(saturate(RdotE), specluerPower);
			float32_t3 spotLightSpecluer = gSpotLights[index].color.xyz * gSpotLights[index].intensity * specularPow * factor * falloffFactor;

			lighting += (spotLightdiffuse + spotLightSpecluer);

		}

		//shadowSpotLight

		uint32_t startShadowSpotLightIndex = lightNum.shadowSpotLight * tileNumber;

		for (int l = 0; l < tileInformation.shadowSpotLightNum; l++) {

			int index = gTBRShadowSpotLightIndex[startShadowSpotLightIndex + l];

			float32_t3 spotLightDirectionOnSurface = normalize(worldPos - gViewProjection.viewPosition);
			float32_t distance = length(gShadowSpotLights[index].position - worldPos);
			float32_t factor = pow(saturate(-distance / gShadowSpotLights[index].distance + 1.0), gShadowSpotLights[index].decay);
			float32_t cosAngle = dot(spotLightDirectionOnSurface, gShadowSpotLights[index].direction);
			float32_t falloffFactor = saturate((cosAngle - gShadowSpotLights[index].cosAngle) / (1.0f - gShadowSpotLights[index].cosAngle));


			//spotLightDiffuse
			float32_t NdotL = dot(normal, -spotLightDirectionOnSurface);
			float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
			float32_t3 spotLightdiffuse = gShadowSpotLights[index].color.xyz * cos * gShadowSpotLights[index].intensity * factor * falloffFactor;


			//spotLightSpecular

			float32_t3 reflectVec = reflect(spotLightDirectionOnSurface, normal);
			float32_t specluerPower = 10.0f;
			float32_t RdotE = dot(reflectVec, viewDirection);
			float32_t specularPow = pow(saturate(RdotE), specluerPower);
			float32_t3 spotLightSpecluer = gShadowSpotLights[index].color.xyz * gShadowSpotLights[index].intensity * specularPow * factor * falloffFactor;

			lighting += (spotLightdiffuse + spotLightSpecluer);

			//影
			float32_t4 wp = float4(worldPos.xyz, 1.0f);
			float32_t4 lightViewPosition = mul(wp, gShadowSpotLights[index].viewProjection);
			float32_t2 shadowMapUV = lightViewPosition.xy / lightViewPosition.w;
			shadowMapUV *= float32_t2(0.5f, -0.5f);
			shadowMapUV += 0.5f;

			if (lightViewPosition.z > 0.0f) {
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
			}

		}

		for (int k = 0; k < lightNum.directionalLight; k++) {

			//directionalLightDiffuse
			float32_t NdotL = dot(normal, -gDirectionLights[k].direction);
			float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
			float32_t3 directionalLightdiffuse = gDirectionLights[k].color.xyz * cos * gDirectionLights[k].intensity;

			//directionalLightSpecular

			float32_t3 reflectVec = reflect(gDirectionLights[k].direction, normal);
			float32_t specluerPower = 10.0f;
			float32_t RdotE = dot(reflectVec, viewDirection);
			float32_t specularPow = pow(saturate(RdotE), specluerPower);
			float32_t3 directionalLightSpecluer = gDirectionLights[k].color.xyz * gDirectionLights[k].intensity * specularPow;

			lighting += (directionalLightdiffuse + directionalLightSpecluer);

			//影
			float32_t4 wp = float4(worldPos.xyz, 1.0f);
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
			}

		}

		for (int m = 0; m < lightNum.areaLight; m++) {

			float32_t segLen = length(gAreaLights[m].diff);
			float32_t3 segDir = gAreaLights[m].diff * (1.0f / segLen);
			float32_t dis = dot(worldPos - gAreaLights[m].origin, segDir);
			dis = clamp(dis, 0.0f, segLen);
			float32_t3 proj = dis * segDir;

			float32_t3 closestPoint = gAreaLights[m].origin + proj;


			float32_t3 areaLightDirection = worldPos - closestPoint;
			float32_t distance = length(areaLightDirection);
			areaLightDirection = areaLightDirection / distance;
			float32_t factor = pow(saturate(-distance / gAreaLights[m].range + 1.0), gAreaLights[m].decay);

			//pointLightDiffuse
			float32_t NdotL = dot(normal, -areaLightDirection);
			float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
			float32_t3 areaLightdiffuse = gAreaLights[m].color.xyz * cos * gAreaLights[m].intensity * factor;

			//pointLightSpecular
			float32_t3 reflectVec = reflect(areaLightDirection, normal);
			float32_t specluerPower = 10.0f;
			float32_t RdotE = dot(reflectVec, viewDirection);
			float32_t specularPow = pow(saturate(RdotE), specluerPower);
			float32_t3 areaLightSpecluer = gAreaLights[m].color.xyz * gAreaLights[m].intensity * specularPow * factor;

			lighting += (areaLightdiffuse + areaLightSpecluer);

		}

		//アンビエント
		float32_t3 ambient = float32_t3(0.0f, 0.0f, 0.0f);

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