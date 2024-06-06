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

#include "ParticleModelData.h"

class DirectXCommon;

class ParticleModel
{
public:

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

	void Initialize();
	void Finalize();
	void PreDraw(CommandContext& commandContext, const ViewProjection& viewProjection);

	void Draw(CommandContext& commandContext, ParticleModelData& bufferData, const Material& material, uint32_t modelHandle = 0);

	
private:
	void CreatePipeline();
private:
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<PipelineState> pipelineState_;

};