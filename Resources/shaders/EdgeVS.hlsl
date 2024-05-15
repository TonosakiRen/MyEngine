
Texture2D<float4> colorTex : register(t0);

struct VSInput {
	uint32_t vertexID : SV_VertexID;
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

struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2 texCenter : TEXCOORD0;
	float32_t2 texLT : TEXCOORD1;
	float32_t2 texMT : TEXCOORD2;
	float32_t2 texRT : TEXCOORD3;
	float32_t2 texLM : TEXCOORD4;
	float32_t2 texRM : TEXCOORD5;
	float32_t2 texLB : TEXCOORD6;
	float32_t2 texMB : TEXCOORD7;
	float32_t2 texRB : TEXCOORD8;
};

VSOutput main(VSInput input) {
	VSOutput output;

	float32_t2 texSize;
	//テクスチャーのサイズ
	colorTex.GetDimensions(texSize.x, texSize.y);

	output.svpos = kPositions[input.vertexID];
	float32_t2 tex = kTexcoords[input.vertexID];

	float32_t2 pixelSize = 1.0f * rcp(texSize);

	//法線
	{
		//真ん中のピクセル
		output.texCenter = tex;
		//右上のピクセル
		output.texLT.xy = tex + float32_t2(pixelSize.x , -pixelSize.y);
		//上のピクセル
		output.texMT.xy = tex + float32_t2(0.0f, -pixelSize.y);
		//左上のピクセル
		output.texRT.xy = tex + float32_t2(-pixelSize.x , -pixelSize.y);
		//右のピクセル
		output.texLM.xy = tex + float32_t2(pixelSize.x,0.0f);
		//左のピクセル
		output.texRM.xy = tex + float32_t2(-pixelSize.x,0.0f);
		//上のピクセル
		output.texLB.xy = tex + float32_t2(pixelSize.x , pixelSize.y);
		//上のピクセル
		output.texMB.xy = tex + float32_t2(0.0f, pixelSize.y);
		//上のピクセル
		output.texRB.xy = tex + float32_t2(-pixelSize.x , pixelSize.y);

	}



	return output;
}