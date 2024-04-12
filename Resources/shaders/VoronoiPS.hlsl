struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct PointNum {
	uint32_t num;
};
ConstantBuffer<PointNum> pointNum  : register(b0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

struct Point {
	float32_t2 pos;
	float32_t4 color;
	uint32_t index;
};
StructuredBuffer<Point> gPoints  : register(t0);

SamplerState smp : register(s0);

float32_t4 main(VSOutput input) : SV_TARGET{
	PixelShaderOutput output;

	float32_t minDistance = 3.402823466e+38F;
	uint32_t index = 0;

	float32_t2 uvPos = input.uv * 1500.0f;

	for (int i = 0; i < pointNum.num; i++) {
		float32_t distance = length(gPoints[i].pos - uvPos);
		if (distance < minDistance) {
			minDistance = distance;
			index = gPoints[i].index;
		}
	}

	output.color = gPoints[index].color;

	return output.color;
}