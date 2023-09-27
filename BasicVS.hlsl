
struct WorldTransform {
	float32_t4x4 world; // ワールド変換行列
};
ConstantBuffer<WorldTransform> gWorldTransform  : register(b0);

struct ViewProjection {
	float32_t4x4 view;       // ビュー変換行列
	float32_t4x4 projection; // プロジェクション変換行列
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b1);
ConstantBuffer<ViewProjection> gShadowViewProjection  : register(b10);

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput {
	float32_t4 svpos : SV_POSITION; // システム用頂点座標
	float32_t3 normal : NORMAL;     // 法線ベクトル
	float32_t2 uv : TEXCOORD;       // uv値
	float32_t4 posInShadow : TEXCOORD1;
};

VSOutput main(float32_t4 pos : POSITION, float32_t3 normal : NORMAL, float32_t2 uv : TEXCOORD) {
	VSOutput output; // ピクセルシェーダーに渡す値
	output.svpos = mul(pos, mul(gWorldTransform.world, mul(gViewProjection.view, gViewProjection.projection)));
	output.normal = normalize(mul(float32_t4(normal.xyz,0.0f), gWorldTransform.world).xyz);
	output.uv = uv;
	output.posInShadow = mul(pos, mul(gWorldTransform.world, mul(gShadowViewProjection.view, gShadowViewProjection.projection)));
	return output;
}