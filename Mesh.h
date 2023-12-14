#pragma once
#include <string>
#include "Mymath.h"
#include "UploadBuffer.h"
#include <vector>
class Mesh
{
	friend class ModelManager;
public:

	struct VertexData {
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

	D3D12_VERTEX_BUFFER_VIEW* GetVbView() {
		return &vbView_;
	}

	D3D12_INDEX_BUFFER_VIEW* GetIbView() {
		return &ibView_;
	}

	UINT GetSize() {
		return static_cast<UINT>(vertices_.size());
	}

	uint32_t GetUv() {
		return uvHandle_;
	}

private:
	UploadBuffer vertexBuffer_;
	UploadBuffer indexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	std::vector<VertexData> vertices_;
	std::vector<uint16_t> indices_;

	std::string name_;
	uint32_t uvHandle_ = 0;
};