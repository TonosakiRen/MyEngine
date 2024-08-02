#include "Common.hlsli"
#include "Lighting.hlsli"
ConstantBuffer<WorldTransform> gWorldTransform  : register(b0);
ConstantBuffer<ViewProjection> gViewProjection  : register(b1);
ConstantBuffer<MeshletInfo> meshletInfo  : register(b4);

struct Vertex {
	float32_t3 pos;
	float32_t3 normal;
	float32_t2 uv;
};
struct MSOutput {
	float32_t4 pos : SV_POSITION;
	float32_t3 normal : NORMAL;
	float32_t2 uv : TEXCOORD;
	uint32_t meshletIndex : CUSTOM_MESHLET_ID;
	float32_t3 worldPosition : POSITION0;
	float32_t depth : TEXCOORD1;
};

StructuredBuffer<Vertex> input : register(t1);
StructuredBuffer<Meshlet> meshlets :register(t2);
ByteAddressBuffer uniqueVertexIndices : register(t3);
StructuredBuffer<uint32_t> primitiveIndices :register(t4);

// 頂点出力情報を取得
MSOutput GetVertexAttribute(uint32_t vertexIndex, uint32_t meshletIndex)
{
	Vertex v = input[vertexIndex];

	MSOutput vout;

	float32_t4 worldPosition = mul(float4(v.pos, 1.0f), gWorldTransform.world);
	float32_t4 viewPosition = mul(worldPosition, gViewProjection.viewProjection);

	// 座標変換
	vout.pos = viewPosition;
	// 法線にワールド行列を適用
	vout.normal = mul(v.normal, (float32_t3x3)gWorldTransform.worldInverseTranspose);

	vout.uv = v.uv;

	vout.worldPosition = worldPosition.xyz;

	vout.depth = viewPosition.z / viewPosition.w;

	// MeshletのIndexを出力
	vout.meshletIndex = meshletIndex;

	return vout;
}

uint32_t3 UnpackPrimitive(uint32_t primitive)
{
	// 32bit から 10 bit ずつ x,y,zに index 情報を格納
	return uint32_t3(primitive & 0x3FF, (primitive >> 10) & 0x3FF, (primitive >> 20) & 0x3FF);
}

// プリミティブ取得
uint32_t3 GetPrimitive(Meshlet m, uint32_t index)
{
	return UnpackPrimitive(primitiveIndices[m.primOffset + index]);
}


// 頂点インデックス取得
uint32_t GetVertexIndex(Meshlet m, in uint32_t vertIndex)
{

	vertIndex = (m.vertOffset + vertIndex) * 4;

	//index 32bit だから 4Bite *
	uint32_t index = uniqueVertexIndices.Load(vertIndex);
	return index;

	//vertIndex = m.vertOffset + vertIndex;

	//// Byte address must be 4-byte aligned.
	//uint wordOffset = (vertIndex & 0x1);
	//uint byteOffset = (vertIndex / 2) * 4;

	//// Grab the pair of 16-bit indices, shift & mask off proper 16-bits.
	//uint indexPair = uniqueVertexIndices.Load(byteOffset);
	//uint index = (indexPair >> (wordOffset * 16)) & 0xffff;

	//return index;
}

struct Payload {
	uint32_t meshletIndices[32];
	uint32_t hitWaveIndices[32];
	uint32_t visibleCount;
};


[numthreads(128, 1, 1)]
[OutputTopology("triangle")]
void main(
	uint32_t gtid : SV_GroupThreadID,
	uint32_t gid : SV_GroupID,
	in payload Payload payload,
	out vertices MSOutput verts[256],
	out indices uint32_t3 tris[256])
{

	uint32_t meshletIndex = payload.meshletIndices[gid];

	if (meshletIndex >= meshletInfo.meshletNum) {
		return;
	}

	Meshlet m = meshlets[meshletIndex];

	SetMeshOutputCounts(m.vertCount, m.primCount);

	if (gtid < m.primCount) {
		tris[gtid] = GetPrimitive(m, gtid);
	}

	if (gtid < m.vertCount) {
		uint32_t vertexIndex = GetVertexIndex(m, gtid);
		verts[gtid] = GetVertexAttribute(vertexIndex, meshletIndex);
	}
}