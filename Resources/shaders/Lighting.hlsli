#define RTWidth 1280
#define RTHeight 720
#define ambient 0.1f

struct TBRInformation {
	uint32_t pointLightNum;
	uint32_t spotLightNum;
	uint32_t shadowSpotLightNum;
	uint32_t pointLightOffset;
	uint32_t spotLightOffset;
	uint32_t shadowSpotLightOffset;
};

struct TileNum{
	uint32_t width;
	uint32_t height;
};

struct DirectionalLight {
	float32_t4 color;
	float32_t3 direction;
	float32_t intensity;
	float32_t4x4 viewProjection;
	uint32_t descriptorIndex;
};

struct AreaLight {
	float32_t4 color;
	float32_t3 origin;
	float32_t3 diff;
	float32_t intensity;
	float32_t range;
	float32_t decay;
	float32_t isActive;
};

struct PointLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t radius;
	float32_t decay;
	float32_t isActive;
};

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

struct LightNum {
	uint32_t  directionalLight;
	uint32_t  pointLight;
	uint32_t  areaLight;
	uint32_t  spotLight;
	uint32_t  shadowSpotLight;
	uint32_t maxInTilePointLight;
};

float32_t3 PointLightLighting(PointLight pointLight ,float32_t3 worldPos,float32_t3 viewDirection,float32_t3 normal){

	float32_t3 pointLightDirection = worldPos - pointLight.position;
	float32_t distance = length(pointLightDirection);
	pointLightDirection = pointLightDirection / distance;
	float32_t factor = pow(saturate(-distance / pointLight.radius + 1.0), pointLight.decay);

	//pointLightDiffuse
	float32_t NdotL = dot(normal, -pointLightDirection);
	float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 pointLightdiffuse = pointLight.color.xyz * cos * pointLight.intensity * factor;

	//pointLightSpecular
	float32_t3 reflectVec = reflect(pointLightDirection, normal);
	float32_t specluerPower = 10.0f;
	float32_t RdotE = dot(reflectVec, viewDirection);
	float32_t specularPow = pow(saturate(RdotE), specluerPower);
	float32_t3 pointLightSpecluer = pointLight.color.xyz * pointLight.intensity * specularPow * factor;

	return (pointLightdiffuse + pointLightSpecluer);
}

float32_t3 SpotLightLighting(SpotLight spotLight ,float32_t3 worldPos,float32_t3 viewDirection,float32_t3 normal){

	float32_t3 spotLightDirectionOnSurface = normalize(worldPos - spotLight.position);
	float32_t distance = length(spotLight.position - worldPos);
	float32_t factor = pow(saturate(-distance / spotLight.distance + 1.0), spotLight.decay);
	float32_t cosAngle = dot(spotLightDirectionOnSurface, spotLight.direction);
	float32_t falloffFactor = saturate((cosAngle - spotLight.cosAngle) / (1.0f - spotLight.cosAngle));


	//spotLightDiffuse
	float32_t NdotL = dot(normal, -spotLightDirectionOnSurface);
	float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 spotLightdiffuse = spotLight.color.xyz * cos * spotLight.intensity * factor * falloffFactor;


	//spotLightSpecular
	float32_t3 reflectVec = reflect(spotLightDirectionOnSurface, normal);
	float32_t specluerPower = 10.0f;
	float32_t RdotE = dot(reflectVec, viewDirection);
	float32_t specularPow = pow(saturate(RdotE), specluerPower);
	float32_t3 spotLightSpecluer = spotLight.color.xyz * spotLight.intensity * specularPow * factor * falloffFactor;

	return (spotLightdiffuse + spotLightSpecluer);
}

float32_t3 ShadowSpotLightLighting(ShadowSpotLight shadowSpotLight ,float32_t3 worldPos,float32_t3 viewDirection,float32_t3 normal){

	float32_t3 spotLightDirectionOnSurface= normalize(worldPos - shadowSpotLight.position);
	float32_t distance = length(shadowSpotLight.position - worldPos);
	float32_t factor = pow(saturate(-distance / shadowSpotLight.distance + 1.0), shadowSpotLight.decay);
	float32_t cosAngle = dot(spotLightDirectionOnSurface, shadowSpotLight.direction);
	float32_t falloffFactor = saturate((cosAngle - shadowSpotLight.cosAngle) / (1.0f - shadowSpotLight.cosAngle));

	//spotLightDiffuse
	float32_t NdotL = dot(normal, -spotLightDirectionOnSurface);
	float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 spotLightdiffuse = shadowSpotLight.color.xyz * cos * shadowSpotLight.intensity * factor * falloffFactor;


	//spotLightSpecular
	float32_t3 reflectVec = reflect(spotLightDirectionOnSurface, normal);
	float32_t specluerPower = 10.0f;
	float32_t RdotE = dot(reflectVec, viewDirection);
	float32_t specularPow = pow(saturate(RdotE), specluerPower);
	float32_t3 spotLightSpecluer = shadowSpotLight.color.xyz * shadowSpotLight.intensity * specularPow * factor * falloffFactor;

	return (spotLightdiffuse + spotLightSpecluer);

}

float32_t3 DirectionalLightLighting(DirectionalLight directionalLight ,float32_t3 worldPos,float32_t3 viewDirection,float32_t3 normal){

	//directionalLightDiffuse
	float32_t NdotL = dot(normal, -directionalLight.direction);
	float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 directionalLightdiffuse = directionalLight.color.xyz * cos * directionalLight.intensity;

	//directionalLightSpecular
	float32_t3 reflectVec = reflect(directionalLight.direction, normal);
	float32_t specluerPower = 10.0f;
	float32_t RdotE = dot(reflectVec, viewDirection);
	float32_t specularPow = pow(saturate(RdotE), specluerPower);
	float32_t3 directionalLightSpecluer = directionalLight.color.xyz * directionalLight.intensity * specularPow;

	return (directionalLightdiffuse + directionalLightSpecluer);

}

float32_t3 AreaLightLighting(AreaLight areaLight ,float32_t3 worldPos,float32_t3 viewDirection,float32_t3 normal){

	if(areaLight.isActive == true){
		float32_t segLen = length(areaLight.diff);
		float32_t3 segDir = areaLight.diff * (1.0f / segLen);
		float32_t dis = dot(worldPos - areaLight.origin, segDir);
		dis = clamp(dis, 0.0f, segLen);
		float32_t3 proj = dis * segDir;

		float32_t3 closestPoint = areaLight.origin + proj;

		float32_t3 areaLightDirection = worldPos - closestPoint;
		float32_t distance = length(areaLightDirection);
		areaLightDirection = areaLightDirection / distance;
		float32_t factor = pow(saturate(-distance / areaLight.range + 1.0), areaLight.decay);

		//pointLightDiffuse
		float32_t NdotL = dot(normal, -areaLightDirection);
		float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		float32_t3 areaLightdiffuse = areaLight.color.xyz * cos * areaLight.intensity * factor;

		//pointLightSpecular
		float32_t3 reflectVec = reflect(areaLightDirection, normal);
		float32_t specluerPower = 10.0f;
		float32_t RdotE = dot(reflectVec, viewDirection);
		float32_t specularPow = pow(saturate(RdotE), specluerPower);
		float32_t3 areaLightSpecluer = areaLight.color.xyz * areaLight.intensity * specularPow * factor;

		return (areaLightdiffuse + areaLightSpecluer);
	}

	return false;

}

int32_t GetTileInformationIndex(float32_t2 pixelPos,TileNum tileNum){

	float32_t2 texSize = float32_t2(1280,720);

	float32_t tileWidth = texSize.x * rcp(tileNum.width);

	float32_t tileHeight = texSize.y * rcp(tileNum.height);

	uint32_t2 tile = uint32_t2(pixelPos.x * rcp(tileWidth), pixelPos.y * rcp(tileHeight));

	//if((tile.y * tileNum.width + tile.x) % 2 == 1){
		//return 0; 
	//}

	return (tile.y * tileNum.width + tile.x);
}