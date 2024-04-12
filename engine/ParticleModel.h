#pragma once
#include <Windows.h>
#include <memory>
#include <vector>

#include "PipelineState.h"
#include "RootSignature.h"
#include "StructuredBuffer.h"
#include "CommandContext.h"

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DirectionalLights.h"
#include "Material.h"
#include "Mesh.h"

class DirectXCommon;

class ParticleModel
{
public:

	const uint32_t kParticleNum;

	enum class RootParameter {
		kWorldTransform,
		kViewProjection,
		kTexture,
		kMaterial,

		parameterNum
	};

	struct VertexData {
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

	struct InstancingBufferData {
		Matrix4x4 matWorld;
	};

	static void StaticInitialize();
	static void Finalize();
	static void PreDraw(CommandContext* commandContext, const ViewProjection& viewProjection);
	static void PostDraw();

	ParticleModel(uint32_t particleNum);

	void Initialize(std::string name);
	void Draw(const std::vector<InstancingBufferData>& bufferData, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });

public:
	Material material_;
private:
	static void CreatePipeline();
private:
	static CommandContext* commandContext_;
	static std::unique_ptr<RootSignature> rootSignature_;
	static std::unique_ptr<PipelineState> pipelineState_;

	StructuredBuffer structuredBuffer_;

	uint32_t modelHandle_ = 0;
};