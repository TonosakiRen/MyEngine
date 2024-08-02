struct Material {
	float32_t4 materialColor;
	float32_t4x4 uvTransfrom;
	int32_t enableLighting;
};

struct ViewProjection {
	float32_t4x4 viewProjection;
	float32_t4x4 inverseViewProjection;
	float32_t4x4 worldMatrix;
	float32_t4x4 billBordMatrix;
	float32_t3 viewPosition;
};

struct WorldTransform {
	float32_t4x4 world;
	float32_t4x4 worldInverseTranspose;
	float32_t scale;
};

struct Plane {
	float32_t3 normal;
	float32_t distance;
};
struct Frustum {
	Plane plane[6];
};

struct Sphere {
	float32_t3 position;
	float32_t radius;
};

struct CullData {
	Sphere sphere;
	uint32_t normalCone;
	float32_t ApexOffset;
};

struct Meshlet {
	uint32_t vertCount;
	uint32_t vertOffset;
	uint32_t primCount;
	uint32_t primOffset;
};

struct MeshletInfo {
	uint32_t meshletNum;
};

struct WaveIndexData {
	uint32_t waveDataNum;
	uint32_t waveIndex[5];
};
struct WaveData {
	float32_t3 position;
	float32_t t;
};
struct WaveParam{
	float32_t hz;
	float32_t period;
	float32_t amplitude;
	float32_t radius;
};

struct Color{
	float32_t4 t;
};

float32_t3 Multiply(float32_t4x4 m, float32_t3 vec) {
	float32_t4 result;
	result = mul(float32_t4(vec, 1.0f), m);
	result.x = result.x * rcp(result.w);
	result.y = result.y * rcp(result.w);
	result.z = result.z * rcp(result.w);
	return result.xyz;
}

float32_t3 MidPoint(float32_t3 point1,float32_t3 point2) {
	return float32_t3((point1.x + point2.x) * 0.5f, (point1.y + point2.y) * 0.5f, (point1.z + point2.z) * 0.5f);
}

float32_t3 FindCenter(float32_t3 p1,float32_t3 p2,float32_t3 p3){
	float32_t3 result;
	result.x = (p1.x + p2.x + p3.x) / 3.0f;
	result.y = (p1.y + p2.y + p3.y) / 3.0f;
	result.z = (p1.z + p2.z + p3.z) / 3.0f;
	return result;
}

float32_t3 GetPosition(float32_t4x4 m){
	return float32_t3(m[3].x, m[3].y, m[3].z);
}
float32_t3 HSVToRGB(in float32_t3 hsv) {
	float32_t4 k = float32_t4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f);
	float32_t3 p = abs(frac(hsv.xxx + k.xyz) * 6.0f - k.www);
	return hsv.z * lerp(k.xxx, clamp(p - k.xxx, 0.0f, 1.0f), hsv.y);
}
