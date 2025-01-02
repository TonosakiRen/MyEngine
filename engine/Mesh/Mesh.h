#pragma once
/**
 * @file Mesh.h
 * @brief Mesh構造体
 */
#include <string>
#include "Mymath.h"
#include "GPUResource/UploadBuffer.h"
#include "GPUResource/DefaultStructuredBuffer.h"
#include "GPUResource/ByteAddressBuffer.h"
#include "Raytracing/BLAS.h"
#include <vector>
#include <memory>
#include <DirectXMesh.h>

class Mesh
{
	friend class ModelManager;
public:
	
	struct VertexData {
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};


	//Getter
	const D3D12_INDEX_BUFFER_VIEW* GetIbView() const {
		return &ibView_;
	}

	const D3D12_VERTEX_BUFFER_VIEW* GetVbView() const {
		return &vbView_;
	}

	const uint32_t GetUv() const{
		return uvHandle_;
	}

	const std::vector<VertexData>& GetVerticies() const {
		return vertices_;
	}
	const DescriptorHandle& GetVerticesSRV() const { return vertexBuffer_.GetSRV(); }

public:
	DefaultStructuredBuffer vertexBuffer_;
	std::vector<VertexData> vertices_;
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	DefaultStructuredBuffer indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	std::vector<uint32_t> indices_;

	DefaultStructuredBuffer meshletBuffer_;
	std::vector<DirectX::Meshlet> meshlets_;
	DefaultBuffer meshletInfo_;

	//Meshlets計算用
	std::vector<DirectX::XMFLOAT3> positions_;

	ByteAddressBuffer uniqueVertexIndexBuffer_;
	std::vector<uint8_t> uniqueVertexIndex;


	DefaultStructuredBuffer primitiveIndicesBuffer_;
	std::vector<DirectX::MeshletTriangle> primitiveIndices_;

	DefaultStructuredBuffer cullDataBuffer_;
	std::vector<DirectX::CullData> cullData_;

	// raytarceing用
	BLAS blasBuffer_;


	std::string name_;
	uint32_t uvHandle_ = 0;
};