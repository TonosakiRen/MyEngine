struct Plane {
	float32_t3 normal;
	float32_t distance;
};
struct Frustum {
	Plane plane[6];
	float32_t3 vertex[8];
};
struct TileInformation {
	uint32_t pointLightNum;
	uint32_t spotLightNum;
	uint32_t shadowSpotLightNum;
};

struct PointLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t radius;
	float32_t decay;
	float32_t isActive;
};

float32_t3 Multiply(float32_t4x4 m, float32_t3 vec) {
	float32_t4 result;
	result = mul(float32_t4(vec, 1.0f), m);
	result.x = result.x * rcp(result.w);
	result.y = result.y * rcp(result.w);
	result.z = result.z * rcp(result.w);
	return result.xyz;
}

float32_t3 MakeNormal(float32_t3 v1, float32_t3 v2, float32_t3 v3) {
	float32_t3 start = v2 - v1;
	float32_t3 end = v3 - v2;

	float32_t3 normal = normalize(cross(start, end));
	return normal;
}

Frustum MakeFrustum(Frustum frustum) {
	//left
	frustum.plane[0].normal = MakeNormal(frustum.vertex[0], frustum.vertex[2], frustum.vertex[6]);
	frustum.plane[0].distance = dot(frustum.plane[0].normal, frustum.vertex[0]);

	//top
	frustum.plane[1].normal = MakeNormal(frustum.vertex[0], frustum.vertex[4], frustum.vertex[5]);
	frustum.plane[1].distance = dot(frustum.plane[1].normal, frustum.vertex[0]);

	//right
	frustum.plane[2].normal = MakeNormal(frustum.vertex[1], frustum.vertex[5], frustum.vertex[7]);
	frustum.plane[2].distance = dot(frustum.plane[2].normal, frustum.vertex[1]);

	//bottom
	frustum.plane[3].normal = MakeNormal(frustum.vertex[2], frustum.vertex[3], frustum.vertex[7]);
	frustum.plane[3].distance = dot(frustum.plane[3].normal, frustum.vertex[2]);

	//front
	frustum.plane[4].normal = MakeNormal(frustum.vertex[0], frustum.vertex[1], frustum.vertex[3]);
	frustum.plane[4].distance = dot(frustum.plane[4].normal, frustum.vertex[0]);

	//back
	frustum.plane[5].normal = MakeNormal(frustum.vertex[4], frustum.vertex[6], frustum.vertex[7]);
	frustum.plane[5].distance = dot(frustum.plane[5].normal, frustum.vertex[4]);

	return frustum;
}


Frustum Multiply(Frustum f, float32_t4x4 m) {

	Frustum result;
	
	result.vertex[0] = Multiply(m, f.vertex[0]);
	result.vertex[1] = Multiply(m, f.vertex[1]);
	result.vertex[2] = Multiply(m, f.vertex[2]);
	result.vertex[3] = Multiply(m, f.vertex[3]);
	result.vertex[4] = Multiply(m, f.vertex[4]);
	result.vertex[5] = Multiply(m, f.vertex[5]);
	result.vertex[6] = Multiply(m, f.vertex[6]);
	result.vertex[7] = Multiply(m, f.vertex[7]);

	result = MakeFrustum(result);

	return result;
}


bool IsHitSphere(Frustum frustum, float32_t3 position, float32_t radius)
{
	uint32_t hitNum = 0;
	//平面の法線と内積をとる
	for (uint32_t i = 0; i < 6; i++) {
		//プラスであれば外側距離を測る,内側の場合マイナス
		float32_t a = dot(frustum.plane[i].normal, position) - frustum.plane[i].distance;
		if (a > radius) {
			return false;
		}
	}

	return true;
}

struct LightNum {
	uint32_t  directionalLight;
	uint32_t  pointLight;
	uint32_t  areaLight;
	uint32_t  spotLight;
	uint32_t  shadowSpotLight;
};

struct ViewProjection {
	float32_t4x4 viewProjection;
	float32_t4x4 inverseViewProjection;
	float32_t4x4 worldMatrix;
	float32_t3 viewPosition;
};

ConstantBuffer<LightNum> lightNum : register(b0);
ConstantBuffer<ViewProjection> gViewProjection  : register(b1);

RWStructuredBuffer<TileInformation> tileInformations : register(u0);
RWStructuredBuffer<uint32_t> pointLightIndex : register(u1);
RWStructuredBuffer<uint32_t> spotLightIndex : register(u2);
RWStructuredBuffer<uint32_t> shadowSpotLightIndex : register(u3);

StructuredBuffer<Frustum> initialTileFrustum : register(t0);
StructuredBuffer<PointLight> gPointLights  : register(t1);
[numthreads(16 * 2, 9 * 2, 1)]
void main( uint32_t2 index : SV_GroupThreadID )
{

	uint32_t tileIndex = index.x + (index.y * (16 * 2));

	tileInformations[tileIndex].pointLightNum = 0;
	tileInformations[tileIndex].spotLightNum = 0;
	tileInformations[tileIndex].shadowSpotLightNum = 0;

	Frustum tileFrustnum = Multiply(initialTileFrustum[tileIndex], gViewProjection.worldMatrix);

	for (uint32_t i = 0; i < lightNum.pointLight;i++) {
		if (gPointLights[i].isActive == true) {
			bool isHit = IsHitSphere(tileFrustnum, gPointLights[i].position, gPointLights[i].radius);
			if (isHit) {
				pointLightIndex[tileIndex * lightNum.pointLight + tileInformations[tileIndex].pointLightNum] = i;
				tileInformations[tileIndex].pointLightNum++;
			}
		}
	}

	for (uint32_t j = 0; j < lightNum.spotLight; j++) {
		spotLightIndex[tileIndex * lightNum.spotLight + tileInformations[tileIndex].spotLightNum] = j;
		tileInformations[tileIndex].spotLightNum++;
	}

	for (uint32_t k = 0; k < lightNum.shadowSpotLight; k++) {
		shadowSpotLightIndex[tileIndex * lightNum.shadowSpotLight + tileInformations[tileIndex].shadowSpotLightNum] = k;
		tileInformations[tileIndex].shadowSpotLightNum++;
	}
}