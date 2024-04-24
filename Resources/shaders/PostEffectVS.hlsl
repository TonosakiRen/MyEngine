struct VSInput {
	uint32_t vertexID : SV_VertexID;
};

struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2 uv : TEXCOORD;
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
	output.svpos = kPositions[input.vertexID];
	output.uv = kTexcoords[input.vertexID];
	return output;
}