struct ViewProjection {
	float32_t4x4 viewProjection;
	float32_t4x4 inverseViewProjection;
	float32_t4x4 worldMatrix;
	float32_t4x4 billBordMatrix;
	float32_t3 viewPosition;
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b0);

struct VSOutput {
	float32_t4 pos : SV_POSITION;
};

VSOutput main(float32_t4 pos : POSITION) {
	VSOutput output; // ピクセルシェーダーに渡す値
	output.pos = mul(pos, gViewProjection.viewProjection);
	return output;
}