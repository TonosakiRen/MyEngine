#pragma once
#include <string>
#include "Mymath.h"
#include "UploadBuffer.h"
#include <vector>
#include <memory>

class Mesh
{
	friend class ModelManager;
public:
	
	struct VertexData {
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

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

private:
	UploadBuffer vertexBuffer_;
	std::vector<VertexData> vertices_;
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	UploadBuffer indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	std::vector<uint32_t> indices_;

	std::string name_;
	uint32_t uvHandle_ = 0;
};