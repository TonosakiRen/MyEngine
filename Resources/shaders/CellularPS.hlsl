struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct Info {
	float32_t maxDistance;
	uint32_t tileWidthNum;
	float32_t tileWidth;
};
ConstantBuffer<Info> info  : register(b0);

struct PixelShaderOutput {
	float32_t color : SV_TARGET0;
};

struct Point {
	float32_t2 pos;
	uint32_t index;
};
StructuredBuffer<Point> gPoints  : register(t0);

SamplerState smp : register(s0);

float32_t4 main(VSOutput input) : SV_TARGET{
	PixelShaderOutput output;

	float32_t minDistance = 3.402823466e+38F;

	float32_t2 uvPos = input.uv * 1024.0f;

	uint32_t2 index = float32_t2(uvPos.x / info.tileWidth , uvPos.y / info.tileWidth);

	if (index.y == info.tileWidthNum) {
		index.y = info.tileWidthNum - 1;
	}

	if (index.x == info.tileWidthNum) {
		index.x = info.tileWidthNum - 1;
	}

	for (int y = -1; y < 2; y++) {
		for (int x = -1; x < 2; x++) {
			float32_t2 saveUvPos = uvPos;
			//通常
			int32_t tmpIndex = (index.y + y) * info.tileWidthNum + (index.x + x);
			//端
			if (index.y == 0 && y == -1 ) {
				tmpIndex += info.tileWidthNum * info.tileWidthNum;
				saveUvPos.y += 1024.0f;
			}
			if (index.x == 0 && x == -1) {
				tmpIndex += info.tileWidthNum;
				saveUvPos.x += 1024.0f;
			}
			if (index.y == (info.tileWidthNum - 1) && y == 1) {
				tmpIndex -= info.tileWidthNum * info.tileWidthNum;
				saveUvPos.y -= 1024.0f;
			}
			if (index.x == (info.tileWidthNum - 1) && x == 1) {
				tmpIndex -= info.tileWidthNum;
				saveUvPos.x -= 1024.0f;
			}

			float32_t distance = length(gPoints[tmpIndex].pos - saveUvPos);
			if (distance < minDistance) {
				minDistance = distance;
			}
		}
	}

	float32_t result = lerp(0.0f, 1.0f, minDistance / (info.tileWidth * 5.0f));

	output.color = result;

	return output.color;
}