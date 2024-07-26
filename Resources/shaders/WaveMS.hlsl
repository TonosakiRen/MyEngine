#define WaveRadius 6.0f

struct WorldTransform {
	float32_t4x4 world;
	float32_t4x4 worldInverseTranspose;
	float32_t scale;
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

struct MeshletInfo {
	uint32_t meshletNum;
};
ConstantBuffer<MeshletInfo> meshletInfo  : register(b3);

struct Time {
	uint32_t t;
};
ConstantBuffer<Time> time  : register(b5);

struct WaveIndexData {
	uint32_t waveDataNum;
	uint32_t waveIndex[5];
};
ConstantBuffer<WaveIndexData> waveIndexData: register(b6);

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
struct Meshlet {
	uint32_t vertCount;
	uint32_t vertOffset;
	uint32_t primCount;
	uint32_t primOffset;
};
StructuredBuffer<Vertex> input : register(t1);
StructuredBuffer<Meshlet> meshlets :register(t2);
ByteAddressBuffer uniqueVertexIndices : register(t3);
StructuredBuffer<uint32_t> primitiveIndices :register(t4);

struct WaveData {
	float32_t3 position;
	float32_t t;
};
StructuredBuffer<WaveData> waveData :register(t6);

struct MSOutput3 {
	MSOutput x;
	MSOutput y;
	MSOutput z;
};

float32_t3 MidPoint(float32_t3 point1,float32_t3 point2) {
	return float32_t3((point1.x + point2.x) * 0.5f, (point1.y + point2.y) * 0.5f, (point1.z + point2.z) * 0.5f);
}

float32_t3 Multiply(float32_t4x4 m, float32_t3 vec) {
	float32_t4 result;
	result = mul(float32_t4(vec, 1.0f), m);
	result.x = result.x * rcp(result.w);
	result.y = result.y * rcp(result.w);
	result.z = result.z * rcp(result.w);
	return result.xyz;
}

float32_t3 WaveVertex(float32_t3 worldPos) {

	for (int i = 0; i < waveIndexData.waveDataNum; i++) {
		WaveData data = waveData[waveIndexData.waveIndex[i]];

		float32_t3 diff = data.position - worldPos;

		float32_t distance = length(diff);

		float32_t3 waveDirection = normalize(diff);

		float32_t influenceRadius = data.t * 6.0f;// 半径6.0fで影響

		//歪み球の中
		if (distance < influenceRadius) {

			//	ドンくらい往復するか
			float32_t hz = 0.2f;
			// 離れ具合の波のできかた
			float32_t period = 2.0f;
			//波の振幅
			float32_t amplitude = 0.05f;

			worldPos += waveDirection * sin(time.t * hz + distance * period) * amplitude;
		}
	}
	return worldPos;
}


// 頂点出力情報を取得
MSOutput GetVertexAttribute(uint32_t vertexIndex, uint32_t meshletIndex)
{
	Vertex v = input[vertexIndex];

	float32_t3 worldPos;

	worldPos = Multiply(gWorldTransform.world, v.pos);

	worldPos = WaveVertex(worldPos);

	float32_t4 viewPosition = mul(float32_t4(worldPos,1.0f), gViewProjection.viewProjection);

	MSOutput vout;
	// 座標変換
	vout.pos = viewPosition;

	// 法線にワールド行列を適用
	vout.normal = mul(v.normal, (float32_t3x3)gWorldTransform.worldInverseTranspose);

	vout.worldPosition = worldPos;

	vout.depth = viewPosition.z / viewPosition.w;
	// MeshletのIndexを出力
	vout.meshletIndex = meshletIndex;

	vout.uv = v.uv;

	return vout;
}

// 頂点出力情報を取得
MSOutput3 GetNewMeshletVertexAttribute(uint32_t vertexIndex1, uint32_t vertexIndex2, uint32_t vertexIndex3, uint32_t meshletIndex)
{
	Vertex v1 = input[vertexIndex1];
	Vertex v2 = input[vertexIndex2];
	Vertex v3 = input[vertexIndex3];

	float32_t3 worldPos1;
	float32_t3 worldPos2;
	float32_t3 worldPos3;

	worldPos1 = Multiply(gWorldTransform.world, v1.pos);
	worldPos2 = Multiply(gWorldTransform.world, v2.pos);
	worldPos3 = Multiply(gWorldTransform.world, v3.pos);

	float32_t3 newWorldPos1 = MidPoint(worldPos1, worldPos2);
	float32_t3 newWorldPos2 = MidPoint(worldPos2, worldPos3);
	float32_t3 newWorldPos3 = MidPoint(worldPos3, worldPos1);

	newWorldPos1 = WaveVertex(newWorldPos1);
	newWorldPos2 = WaveVertex(newWorldPos2);
	newWorldPos3 = WaveVertex(newWorldPos3);

	MSOutput3 vout;

	float32_t4 viewPosition1 = mul(float32_t4(newWorldPos1,1.0f), gViewProjection.viewProjection);
	float32_t4 viewPosition2 = mul(float32_t4(newWorldPos2,1.0f), gViewProjection.viewProjection);
	float32_t4 viewPosition3 = mul(float32_t4(newWorldPos3,1.0f), gViewProjection.viewProjection);
	
	// 座標変換
	vout.x.pos = viewPosition1;
	vout.y.pos = viewPosition2;
	vout.z.pos = viewPosition3;

	// 法線にワールド行列を適用
	vout.x.normal = mul(v1.normal, (float32_t3x3)gWorldTransform.worldInverseTranspose);
	vout.y.normal = mul(v2.normal, (float32_t3x3)gWorldTransform.worldInverseTranspose);
	vout.z.normal = mul(v3.normal, (float32_t3x3)gWorldTransform.worldInverseTranspose);

	vout.x.worldPosition = newWorldPos1;
	vout.y.worldPosition = newWorldPos2;
	vout.z.worldPosition = newWorldPos3;

	// MeshletのIndexを出力
	vout.x.meshletIndex = meshletIndex;
	vout.y.meshletIndex = meshletIndex;
	vout.z.meshletIndex = meshletIndex;

	vout.x.depth = viewPosition1.z / viewPosition1.w;
	vout.y.depth = viewPosition2.z / viewPosition2.w;
	vout.z.depth = viewPosition3.z / viewPosition3.w;

	vout.x.uv = v1.uv;
	vout.y.uv = v2.uv;
	vout.z.uv = v3.uv;

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

	Meshlet m;
	uint32_t meshletIndex;
	uint32_t tmp = gid;

	if (gid >= payload.visibleCount) {
		tmp -= payload.visibleCount;
	}

	meshletIndex = payload.meshletIndices[tmp];

	m = meshlets[meshletIndex];

	SetMeshOutputCounts(m.vertCount, m.primCount);

	//通常のメッシュレットの処理
	if (gid < payload.visibleCount) {
		

		if (gtid < m.primCount) {
			tris[gtid] = GetPrimitive(m, gtid);
		}

		if (gtid < m.vertCount) {
			uint32_t vertexIndex = GetVertexIndex(m, gtid);
			verts[gtid] = GetVertexAttribute(vertexIndex, meshletIndex);
		}
		
	}//増やしたメッシュレット
	else {

		if (gtid < m.primCount) {
			if (gtid < m.primCount) {
				uint32_t3 triangleIndex = GetPrimitive(m, gtid);
				tris[gtid] = triangleIndex;

				uint32_t vertexIndex1 = GetVertexIndex(m, triangleIndex.x);
				uint32_t vertexIndex2 = GetVertexIndex(m, triangleIndex.y);
				uint32_t vertexIndex3 = GetVertexIndex(m, triangleIndex.z);

				MSOutput3 newVertex = GetNewMeshletVertexAttribute(vertexIndex1,vertexIndex2,vertexIndex3,meshletIndex);

				verts[triangleIndex.x] = newVertex.x;
				verts[triangleIndex.y] = newVertex.y;
				verts[triangleIndex.z] = newVertex.z;
			
			}
		}
		
	
		//仮
		/*if (gtid < m.primCount) {
			tris[gtid] = GetPrimitive(m, gtid);
		}

		if (gtid.x < m.vertCount) {
			uint32_t vertexIndex = GetVertexIndex(m, gtid);
			verts[gtid] = GetVertexAttribute(vertexIndex, meshletIndex);
		}*/

	}
	
}
