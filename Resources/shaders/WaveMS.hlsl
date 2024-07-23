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
	uint32_t meshletIndex : CUSTOM_MESHLET_ID;;
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

float32_t3 Multiply(float32_t4x4 m, float32_t3 vec) {
	float32_t4 result;
	result = mul(float32_t4(vec, 1.0f), m);
	result.x = result.x * rcp(result.w);
	result.y = result.y * rcp(result.w);
	result.z = result.z * rcp(result.w);
	return result.xyz;
}

// 頂点出力情報を取得
MSOutput GetVertexAttribute(uint32_t vertexIndex, uint32_t meshletIndex)
{
	Vertex v = input[vertexIndex];

	float32_t3 worldPos;

	worldPos = Multiply(gWorldTransform.world, v.pos);

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


	MSOutput vout;
	// 座標変換
	vout.pos = mul(float32_t4(worldPos, 1.0f), gViewProjection.viewProjection);

	// 法線にワールド行列を適用
	vout.normal = mul(v.normal, (float32_t3x3)gWorldTransform.worldInverseTranspose);
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
	uint32_t visibleCount;
	int32_t isHitWaveIndex[32 * 5];
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

	//増やしたMESHLETの処理
	/*if (gid > payload.visibleCount) {
		for (uint32_t i = 0; i < 32; i++) {
			for (uint32_t j = 0; j < 5; j++) {
				int32_t waveIndex = payload.isHitWaveIndex[i * 5 + j];
				if (waveIndex >= 0) {
					uint32_t waveIndex = waveIndexData.waveIndex[i];

					if (IsHit(cullData[dtid].sphere, waveData[waveIndex].position, WaveRadius)) {
						payload.isHitWaveIndex[index * 5 + i] = waveIndex;
					}
					else {
						payload.isHitWaveIndex[index * 5 + i] = -1;
					}
				}
			}
		}

		return;
	}
	else {*/
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
	//}
}
