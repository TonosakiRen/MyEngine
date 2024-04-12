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

floa32_t3 Multiply(float32_t4x4 m, float32_t3x3 vec) {
	float32_t4 result;
	result = mul(float32_t4(vec, 1.0f), m);
	result.x = result.x / result.w;
	result.y = result.x / result.w;
	result.z = result.x / result.w;
	return result.xyz;
}

float32_t3 MakeNormal(float32_t3 v1, float32_t3 v2, float32_t3 v3) {
	float32_t3 start = v2 - v1;
	float32_t3 end = v3 - v2;

	float32_t3 normal = normalize(cross(start, end));
	return normal;
}

Frustum MakeFrustum(float32_t3 frontLeftTop, float32_t3 frontRightTop, float32_t3 frontLeftBottom, float32_t3 frontRightBottom
	, float32_t3 backLeftTop, float32_t3 backRightTop, float32_t3 backLeftBottom, float32_t3 backRightBottom) {
	Frustum frustum;
	//left
	frustum.plane[0].normal = MakeNormal(frontLeftTop, frontLeftBottom, backLeftBottom);
	frustum.plane[0].distance = dot(frustum.plane[0].normal, frontLeftTop);

	//top
	frustum.plane[1].normal = MakeNormal(frontLeftTop, backLeftTop, backRightTop);
	frustum.plane[1].distance = dot(frustum.plane[1].normal, frontLeftTop);

	//right
	frustum.plane[2].normal = MakeNormal(frontRightTop, backRightTop, backRightBottom);
	frustum.plane[2].distance = dot(frustum.plane[2].normal, frontRightTop);

	//bottom
	frustum.plane[3].normal = MakeNormal(frontLeftBottom, frontRightBottom, backRightBottom);
	frustum.plane[3].distance = dot(frustum.plane[3].normal, frontLeftBottom);

	//front
	frustum.plane[4].normal = MakeNormal(frontLeftTop, frontRightTop, frontRightBottom);
	frustum.plane[4].distance = dot(frustum.plane[4].normal, frontLeftTop);

	//back
	frustum.plane[5].normal = MakeNormal(backRightTop, backLeftTop, backLeftBottom);
	frustum.plane[5].distance = dot(frustum.plane[5].normal, backRightTop);

	frustum.vertex[0] = frontLeftTop;
	frustum.vertex[1] = frontRightTop;
	frustum.vertex[2] = frontLeftBottom;
	frustum.vertex[3] = frontRightBottom;
	frustum.vertex[4] = backLeftTop;
	frustum.vertex[5] = backRightTop;
	frustum.vertex[6] = backLeftBottom;
	frustum.vertex[7] = backRightBottom;
	return frustum;
}

Frustum Multiply(Frustum f,float32_t4x4 m) {

	Frustum result;
	float32_t4 vertex[8];
	vertex[0] = Multiply(m, f.vertex[0]);
	vertex[1] = Multiply(m, f.vertex[1]);
	vertex[2] = Multiply(m, f.vertex[2]);
	vertex[3] = Multiply(m, f.vertex[3]);
	vertex[4] = Multiply(m, f.vertex[4]);
	vertex[5] = Multiply(m, f.vertex[5]);
	vertex[6] = Multiply(m, f.vertex[6]);
	vertex[7] = Multiply(m, f.vertex[7]);

	MakeFrustum(result);

	return result;
}


RWStructuredBuffer<TileInformation> tileInformations : register(u0);
RWStructuredBuffer<uint32_t> pointLightIndex : register(u1);
RWStructuredBuffer<uint32_t> spotLightIndex : register(u2);
RWStructuredBuffer<uint32_t> shadowSpotLightIndex : register(u3);

RWStructuredBuffer<Frustum> Output : register(t0);
[numthreads(16, 9, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	
}