
struct WorldTransform {
	float32_t4x4 world;
	float32_t4x4 worldInverseTranspose;
};
ConstantBuffer<WorldTransform> gWorldTransform  : register(b0);

struct ViewProjection {
	float32_t4x4 viewProjection;
	float32_t4x4 inverseViewProjection;
	float32_t4x4 worldMatrix;
	float32_t3 viewPosition;
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b1);

struct Well {
	float32_t4x4 skeletonSpaceMatrix;
	float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};
StructuredBuffer<Well> gMatrixPalette : register(t1);

struct VSOutput {
	float32_t4 pos : SV_POSITION;
	float32_t3 normal : NORMAL;
	float32_t2 uv : TEXCOORD;
};

struct VSInput {
	float32_t3 pos : POSITION;
	float32_t3 normal : NORMAL;
	float32_t2 uv : TEXCOORD;
	float32_t4 weight : WEIGHT;
	int32_t4 index : INDEX;
};

struct Skinned {
	float32_t4 pos;
	float32_t3 normal;
};

Skinned Skinning(VSInput input) {
	Skinned skinned;
	//位置の変換
	skinned.pos = mul(float32_t4(input.pos, 1.0f), gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
	skinned.pos += mul(float32_t4(input.pos, 1.0f), gMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
	skinned.pos += mul(float32_t4(input.pos, 1.0f), gMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
	skinned.pos += mul(float32_t4(input.pos, 1.0f), gMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
	skinned.pos.w = 1.0f;

	//法線の変換
	skinned.normal = mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.x;
	skinned.normal += mul(input.normal,(float32_t3x3)gMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMatrix) * input.weight.y;
	skinned.normal += mul(input.normal,(float32_t3x3)gMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMatrix) * input.weight.z;
	skinned.normal += mul(input.normal,(float32_t3x3)gMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMatrix) * input.weight.w;
	skinned.normal = normalize(skinned.normal);

	return skinned;
}

VSOutput main(VSInput input) {//入力頂点は当然SkeletonSpace
	VSOutput output; // ピクセルシェーダーに渡す値
	Skinned skinned = Skinning(input);//まずSkinning計算を行って、Skinninig後の頂点情報を手に入れる、ここでの頂点もSkeletonSpace
	//SKinnig結果を使って変換
	output.pos = mul(mul(skinned.pos, gWorldTransform.world), gViewProjection.viewProjection);
	output.normal = mul(input.normal, (float32_t3x3)gWorldTransform.worldInverseTranspose);
	output.uv = input.uv;
	return output;
}