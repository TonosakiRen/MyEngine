struct WorldTransform {
	float32_t4x4 world; // ワールド変換行列
};
ConstantBuffer<WorldTransform> gWorldTransform  : register(b0);

struct ViewProjection {
	float32_t4x4 view;       // ビュー変換行列
	float32_t4x4 projection; // プロジェクション変換行列
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b1);

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput {
	float32_t4 svpos : SV_POSITION; // システム用頂点座標
};

struct VSInput {
	float32_t3 pos : POSITION;
};

VSOutput main(VSInput input) {
	VSOutput output; // ピクセルシェーダーに渡す値
	output.svpos = mul(float32_t4(input.pos.xyz,1.0f), mul(gWorldTransform.world, mul(gViewProjection.view, gViewProjection.projection)));
	return output;
}