#include "Common.hlsli"
#include "Lighting.hlsli"
struct Time {
	uint32_t t;
};
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
struct MSOutput3 {
	MSOutput x;
	MSOutput y;
	MSOutput z;
};
ConstantBuffer<WorldTransform> gWorldTransform  : register(b0);
ConstantBuffer<ViewProjection> gViewProjection  : register(b1);
ConstantBuffer<Time> time  : register(b3);
ConstantBuffer<MeshletInfo> meshletInfo  : register(b4);
ConstantBuffer<WaveParam> waveParam: register(b7);

StructuredBuffer<Vertex> input : register(t1);
StructuredBuffer<Meshlet> meshlets :register(t2);
ByteAddressBuffer uniqueVertexIndices : register(t3);
StructuredBuffer<uint32_t> primitiveIndices :register(t4);
StructuredBuffer<WaveData> waveData :register(t6);
StructuredBuffer<uint32_t> waveIndexData :register(t7);


float32_t3 WaveVertex(float32_t3 worldPos) {

	float32_t3 result = worldPos;

	for (int i = 0; i < waveIndexData[0]; i++) {

		WaveData data = waveData[waveIndexData[i + 1]];

		float32_t3 diff = data.position - worldPos;

		float32_t distance = length(diff);

		float32_t3 waveDirection = normalize(float32_t3(0.0f,1.0f,0.0f));

		float32_t influenceRadius = data.t * waveParam.radius;// 半径6.0fで影響

		float32_t amplitudeT = distance * rcp( waveParam.radius);

		//歪み球の中
		if (distance < influenceRadius) {
			result += waveDirection * sin( PI * 0.5f + time.t * waveParam.hz + distance * waveParam.period) * waveParam.amplitude;
		}
	}
	return result;
}

float32_t3 WaveNormal(float32_t3 worldPos) {

	float32_t3 normal = float32_t3(0.0f,0.0f,0.0f);

	uint32_t isWaveNum = 0;

	for (int i = 0; i < waveIndexData[0]; i++) {
		WaveData data = waveData[waveIndexData[i + 1]];

		float32_t2 diff = worldPos.xz - data.position.xz;

		float32_t distance = length(diff);

		float32_t3 waveDirection = normalize(float32_t3(0.0f,1.0f,0.0f));

		float32_t influenceRadius = data.t * waveParam.radius;// 半径6.0fで影響

		//歪み球の中
		if (distance < influenceRadius) {
			 isWaveNum++;
			 float x = -(waveParam.amplitude * waveParam.period * diff.x * cos(distance * waveParam.period + PI * 0.5f + time.t * waveParam.hz)) / distance;
			 float y = 1.0f;
			 float z = -(waveParam.amplitude * waveParam.period * diff.y * cos( distance * waveParam.period + PI * 0.5f + time.t * waveParam.hz)) / distance;
		  	 normal += normalize(float32_t3(x,y,z));
		}

	}

	for(int j = 0; j < isWaveNum; j++){
		normal = normal / isWaveNum;
		normal = normalize(normal);
	}

	if(isWaveNum == 0){
		normal = float32_t3(0.0f,1.0f,0.0f);
	}

	return normal;
}


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

// 頂点出力情報を取得
MSOutput3 GetNewMeshletVertexAttribute(uint32_t vertexIndex1, uint32_t vertexIndex2, uint32_t vertexIndex3, uint32_t meshletIndex,uint32_t triangleIndex)
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

	float32_t3 midPoint1 = MidPoint(worldPos1, worldPos2);
	float32_t3 midPoint2 = MidPoint(worldPos2, worldPos3);
	float32_t3 midPoint3 = MidPoint(worldPos3, worldPos1);

	float32_t3 midNormal1 = normalize(v1.normal + v2.normal);
	float32_t3 midNormal2 = normalize(v2.normal + v3.normal);
	float32_t3 midNormal3 = normalize(v3.normal + v1.normal);

	float32_t3 newWorldPos1;
	float32_t3 newWorldPos2;
	float32_t3 newWorldPos3;

	MSOutput3 vout;

	if(triangleIndex == 0){
		newWorldPos1 = worldPos1;
		newWorldPos2 = midPoint1;
		newWorldPos3 = midPoint3;

	}else if(triangleIndex == 1){
		newWorldPos1 = midPoint1;
		newWorldPos2 = worldPos2;
		newWorldPos3 = midPoint2;

	}else if(triangleIndex == 2){
		newWorldPos1 = midPoint3;
		newWorldPos2 = midPoint1;
		newWorldPos3 = midPoint2;

	}else {
		newWorldPos1 = midPoint3;
		newWorldPos2 = midPoint2;
		newWorldPos3 = worldPos3;
	}

	newWorldPos1 = WaveVertex(newWorldPos1);
	newWorldPos2 = WaveVertex(newWorldPos2);
	newWorldPos3 = WaveVertex(newWorldPos3);

	vout.x.normal  = WaveNormal(newWorldPos1);
	vout.y.normal = WaveNormal(newWorldPos2);
	vout.z.normal  = WaveNormal(newWorldPos3);

	float32_t4 viewPosition1 = mul(float32_t4(newWorldPos1,1.0f), gViewProjection.viewProjection);
	float32_t4 viewPosition2 = mul(float32_t4(newWorldPos2,1.0f), gViewProjection.viewProjection);
	float32_t4 viewPosition3 = mul(float32_t4(newWorldPos3,1.0f), gViewProjection.viewProjection);
	
	// 座標変換
	vout.x.pos = viewPosition1;
	vout.y.pos = viewPosition2;
	vout.z.pos = viewPosition3;

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
		meshletIndex = payload.hitWaveIndices[tmp / 4];
	}else{
		meshletIndex = payload.meshletIndices[tmp];
	}

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



				MSOutput3 newVertex = GetNewMeshletVertexAttribute(vertexIndex1,vertexIndex2,vertexIndex3,meshletIndex,tmp % 4);

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