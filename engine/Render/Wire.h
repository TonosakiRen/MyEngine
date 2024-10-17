#pragma once
#include <Windows.h>
#include <memory>
#include <d3d12.h>
#include <vector>

#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"
#include "GPUResource/UploadBuffer.h"
#include "Graphics/CommandContext.h"

#include "GameComponent/ViewProjection.h"
#include "Mesh/Mesh.h"
#include "Animation/Skeleton.h"
#include "Mymath.h"

#include <DirectXMesh.h>

class DirectXCommon;

class Wire
{
public:

	static const uint32_t kLineNum = 100000;

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
	static void Draw(const DirectX::BoundingSphere& boundingSphere, const WorldTransform& worldTransform);
	static void Draw(const Sphere& sphere, const WorldTransform& worldTransform);

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

