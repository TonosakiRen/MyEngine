#pragma once
#include <Windows.h>
#include <memory>
#include <vector>
#include <d3d12.h>

#include "PipelineState.h"
#include "RootSignature.h"
#include "DescriptorHandle.h"
#include "UploadBuffer.h"

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DirectionalLights.h"
#include "Material.h"
#include "Mesh.h"

class DirectXCommon;

class PostEffect
{
public:
	enum class RootParameter {
		Constant,
		kTexture,

		ParameterNum
	};

	struct VertexData {
		Vector4 pos;
		Vector2 uv;
	};

	void Initialize();

	void Draw(DescriptorHandle srvHandle, ID3D12GraphicsCommandList* sCommandList);

private:
	void CreateMesh();
	void CreatePipeline();
private:
	RootSignature rootSignature_;
	PipelineState pipelineState_;

	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	std::vector<VertexData> vertices_;
	std::vector<uint16_t> indices_;
	UploadBuffer vertexBuffer_;
	UploadBuffer indexBuffer_;

	float constant_ = 1.0f;
};

