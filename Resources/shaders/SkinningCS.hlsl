struct Vertex {
	float32_t3 position;
	float32_t3 normal;
	float32_t2 uv;
};
struct VertexInfluence {
	float32_t4 weight;
	int32_t4 index;
};
struct SkinningInformation {
	uint32_t numVertices;
};
struct Well {
	float32_t4x4 skeletonSpaceMatrix;
	float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};
StructuredBuffer<Well> matrixPalette : register(t0);
StructuredBuffer<Vertex> inputVertices : register(t1);
StructuredBuffer<VertexInfluence> influences : register(t2);
RWStructuredBuffer<Vertex> skinnedVertices : register(u0);
ConstantBuffer<SkinningInformation> skinningInformation  : register(b0);

struct Skinned {
	float32_t4 position;
	float32_t3 normal;
};

Skinned Skinning(Vertex input , uint32_t vertexIndex) {
	Skinned skinned;
	//位置の変換
	skinned.position = mul(float32_t4(input.position, 1.0f), matrixPalette[influences[vertexIndex].index.x].skeletonSpaceMatrix) * influences[vertexIndex].weight.x;
	skinned.position += mul(float32_t4(input.position, 1.0f), matrixPalette[influences[vertexIndex].index.y].skeletonSpaceMatrix) * influences[vertexIndex].weight.y;
	skinned.position += mul(float32_t4(input.position, 1.0f), matrixPalette[influences[vertexIndex].index.z].skeletonSpaceMatrix) * influences[vertexIndex].weight.z;
	skinned.position += mul(float32_t4(input.position, 1.0f), matrixPalette[influences[vertexIndex].index.w].skeletonSpaceMatrix) * influences[vertexIndex].weight.w;
	skinned.position.w = 1.0f;

	//法線の変換
	skinned.normal = mul(input.normal, (float32_t3x3)matrixPalette[influences[vertexIndex].index.x].skeletonSpaceInverseTransposeMatrix) * influences[vertexIndex].weight.x;
	skinned.normal += mul(input.normal, (float32_t3x3)matrixPalette[influences[vertexIndex].index.y].skeletonSpaceInverseTransposeMatrix) * influences[vertexIndex].weight.y;
	skinned.normal += mul(input.normal, (float32_t3x3)matrixPalette[influences[vertexIndex].index.z].skeletonSpaceInverseTransposeMatrix) * influences[vertexIndex].weight.z;
	skinned.normal += mul(input.normal, (float32_t3x3)matrixPalette[influences[vertexIndex].index.w].skeletonSpaceInverseTransposeMatrix) * influences[vertexIndex].weight.w;
	skinned.normal = normalize(skinned.normal);

	return skinned;
}

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint32_t vertexIndex = DTid.x;
	if (vertexIndex < skinningInformation.numVertices) {
		//skinning計算
		Vertex input = inputVertices[vertexIndex];
		VertexInfluence influence = influences[vertexIndex];

		Vertex skinned;
		skinned.uv = input.uv;

		Skinned tmp = Skinning(input, vertexIndex);

		skinned.position = tmp.position.xyz;
		skinned.normal = tmp.normal;

		skinnedVertices[vertexIndex] = skinned;
	}
}