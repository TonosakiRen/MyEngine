#pragma once
#include <Windows.h>
#include <memory>
#include <d3d12.h>
#include <vector>

#include "PipelineState.h"
#include "RootSignature.h"
#include "UploadBuffer.h"
#include "CommandContext.h"

#include "ViewProjection.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "Mymath.h"

class DirectXCommon;

class Wire
{
public:

	static const uint32_t kLineNum = 5000;

	enum class RootParameter {
		kViewProjection, 

		parameterNum
	};

	void Initialize();
	static void Finalize();
	void AllDraw(CommandContext& commandContext, const ViewProjection& viewProjection);

	static void Draw(const std::vector<Vector3>& vertices);
	static void Draw(const Vector3& start, const Vector3& end);
	static void Draw(const Skeleton& skeleton, const WorldTransform& worldTransform);
	static void Draw(const Frustum& frustum);

private: 
	void CreatePipeline();
public:
	
private:
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;
	std::unique_ptr<D3D12_VERTEX_BUFFER_VIEW> vertexBufferView_;
	std::unique_ptr<UploadBuffer> vertexBuffer_;
	static std::vector<Vector3> vertices_;
};

