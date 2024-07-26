#define RTWidth 1280
#define RTHeight 720
#define MY_TEXTURE2D_SPACE space1
Texture2D<float4> Texture2DTable[]  : register(t0, MY_TEXTURE2D_SPACE);
Texture2D<float4> tex : register(t1);
SamplerState smp : register(s0);

RWStructuredBuffer<uint32_t> gTBRPointLightIndex  : register(u1);
RWStructuredBuffer<uint32_t> gTBRSpotLightIndex  : register(u2);
RWStructuredBuffer<uint32_t> gTBRShadowSpotLightIndex  : register(u3);

struct ViewProjection {
	float32_t4x4 viewProjection;
	float32_t4x4 inverseViewProjection;
	float32_t4x4 worldMatrix;
	float32_t4x4 billBordMatrix;
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
StructuredBuffer<DirectionLight> gDirectionLights  : register(t2);

struct PointLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t radius;
	float32_t decay;
	float32_t isActive;
};
StructuredBuffer<PointLight> gPointLights  : register(t3);

struct AreaLight {
	float32_t4 color;
	float32_t3 origin;
	float32_t3 diff;
	float32_t intensity;
	float32_t range;
	float32_t decay;
	float32_t isActive;
};
StructuredBuffer<AreaLight> gAreaLights  : register(t4);

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
StructuredBuffer<SpotLight> gSpotLights  : register(t5);

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
StructuredBuffer<ShadowSpotLight> gShadowSpotLights  : register(t6);

struct TileNum{
	float32_t2 param;
};
ConstantBuffer<TileNum> tileNum : register(b2);

struct Material {
	float32_t4 materialColor; //Materialの色
	float32_t4x4 uvTransfrom;//uvtransform
	int32_t enableLighting; //lighitngするか
};
ConstantBuffer<Material> gMaterial  : register(b1);

struct TBRInformation {
	uint32_t pointLightNum;
	uint32_t spotLightNum;
	uint32_t shadowSpotLightNum;

	uint32_t pointLightOffset;
	uint32_t spotLightOffset;
	uint32_t shadowSpotLightOffset;
};
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
		float32_t2 texSize = float32_t2(RTWidth,RTHeight);

		float32_t tileWidth = texSize.x * rcp(tileNum.param.x);

		float32_t tileHeight = texSize.y * rcp(tileNum.param.y);

		float32_t2 pixelPos = input.pos.xy;

		uint32_t2 tile = uint32_t2(pixelPos.x * rcp(tileWidth), pixelPos.y * rcp(tileHeight));

		uint32_t tileNumber = tile.y * tileNum.param.x + tile.x;

		TBRInformation tileInformation = gTBRInformation[tileNumber];

		float32_t3 viewDirection = normalize(gViewProjection.viewPosition - worldPos);

		for (int i = 0; i < tileInformation.pointLightNum; i++) {

			int index = gTBRPointLightIndex[tileInformation.pointLightOffset + i];

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

		for (int j = 0; j < tileInformation.spotLightNum; j++) {

			int index = gTBRSpotLightIndex[tileInformation.spotLightOffset + j];

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

		for (int l = 0; l < tileInformation.shadowSpotLightNum; l++) {

			int index = gTBRShadowSpotLightIndex[tileInformation.shadowSpotLightOffset + l];

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

		lighting *= shading;

		lighting += ambient;

		color.xyz *= lighting;

	}

	output.color = color;
	return output;
}