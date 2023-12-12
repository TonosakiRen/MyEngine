#pragma once
#include <string>
#include "Mymath.h"
#include "UploadBuffer.h"
#include <vector>
class Mesh
{
public:

	struct VertexData {
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

	void Create(std::string name);

	D3D12_VERTEX_BUFFER_VIEW* GetVbView() {
		return &vbView_;
	}

	UINT GetSize() {
		return static_cast<UINT>(vertices_.size());
	}

	uint32_t GetUv() {
		return uvHandle_;
	}

private:
	UploadBuffer vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	std::vector<VertexData> vertices_;

	std::string name_;
	uint32_t uvHandle_ = 0;
};