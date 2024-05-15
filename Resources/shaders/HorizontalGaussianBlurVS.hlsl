Texture2D<float4> src_ : register(t0);

struct VSInput {
	uint32_t vertexID : SV_VertexID;
};

struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t4 texcoord[8] : TEXCOORD0;
};

static const uint32_t kNumVertex = 3;
static const float32_t4 kPositions[kNumVertex] = {
	{-1.0f,1.0f,0.0f,1.0f}, //左上
	{3.0f,1.0f,0.0f,1.0f}, // 右上
	{-1.0f,-3.0f,0.0f,1.0f} //左下
};
static const float32_t2 kTexcoords[kNumVertex] = {
	{0.0f,0.0f}, //左上
	{2.0f,0.0f}, //右下
	{0.0f,2.0f} //左下
};

VSOutput main(VSInput input) {
	VSOutput output;

	float32_t2 textureSize;
	float level;
	src_.GetDimensions(0, textureSize.x, textureSize.y, level);

	float32_t2 base = kTexcoords[input.vertexID];

	float32_t2 texelSize = float32_t2(1.0f / textureSize.x, 0.0f);

	output.svpos = float32_t4(lerp(float32_t2(-1.0f, 1.0f), float32_t2(1.0f, -1.0f), base), 0.0f, 1.0f);

	float32_t2 offset = texelSize;
	float32_t4 basebase = float32_t4(base, base);

	for (uint i = 0; i < 8; ++i) {
		output.texcoord[i].xy = offset;
		output.texcoord[i].zw = output.texcoord[i].xy * -1.0f;
		output.texcoord[i] += basebase;
		offset = texelSize * 2.0f;
	}

	return output;
}