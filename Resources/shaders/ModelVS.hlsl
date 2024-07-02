
struct WorldTransform {
	float32_t4x4 world;
	float32_t4x4 worldInverseTranspose;
};
ConstantBuffer<WorldTransform> gWorldTransform  : register(b0);

struct ViewProjection {
	float32_t4x4 viewProjection;
	float32_t4x4 inverseViewProjection;
	float32_t4x4 worldMatrix;
	float32_t4x4 billBordMatrix;
	float32_t3 viewPosition;
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b1);

struct VSOutput {
	float32_t4 pos : SV_POSITION;
	float32_t3 normal : NORMAL;
	float32_t2 uv : TEXCOORD;
};

VSOutput main(float32_t3 pos : POSITION, float32_t3 normal : NORMAL, float32_t2 uv : TEXCOORD) {
	VSOutput output; // ピクセルシェーダーに渡す値
	output.pos = mul(float32_t4(pos,1.0f), mul(gWorldTransform.world, gViewProjection.viewProjection));
	output.normal = mul(normal, (float32_t3x3)gWorldTransform.worldInverseTranspose);
	output.uv = uv;
	return output;
}