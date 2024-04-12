#pragma once
#include <Windows.h>
#include <memory>
#include <vector>

#include "PipelineState.h"
#include "RootSignature.h"
#include "UploadBuffer.h"
#include "CommandContext.h"

#include "ViewProjection.h"

class DirectXCommon;

class Wire
{
public:
	enum class RootParameter {
		kViewProjection,

		parameterNum
	};

	static void StaticInitialize();
	static void Finalize();
	static void PreDraw(CommandContext* commandContext, const ViewProjection& viewProjection);
	static void PostDraw();

	static void Draw(const Vector3& start,const Vector3& end);

private:
	static void CreatePipeline();
	static void CreateVertex();
private:
	static CommandContext* commandContext_;
	static std::unique_ptr<RootSignature> rootSignature_;
	static std::unique_ptr<PipelineState> pipelineState_;
	static uint32_t reservedCount_;

	static D3D12_VERTEX_BUFFER_VIEW vbView_;
	static std::vector<Vector3> vertices_;
	static UploadBuffer vertexBuffer_;
};

