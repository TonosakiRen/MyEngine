#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <d3d12.h>

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "ModelManager.h"

#include "CommandSignature.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "AppendBuffer.h"

class Calling
{
public:


    struct CallingInformation {
        Matrix4x4 matWorld;
        Sphere sphere;
    };

    struct ArgumentBuffer {
        D3D12_DRAW_INDEXED_ARGUMENTS arguments;
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW indexBufferView;
        WorldTransform::ConstBufferData worldTransform;
        Material::ConstBufferData material;
        
    };

	void Initialize();
	void SetViewProjection(const ViewProjection* viewProjection) {
		currentViewProjection = viewProjection;
	}

	ModelManager* modelManager = nullptr;
	const ViewProjection* currentViewProjection = nullptr;

	bool isDraw(const uint32_t modelHandle,const WorldTransform& worldTransform);
	uint32_t GetTileIndex();

	bool IsFrustumSphereCollision(const Frustum& frustum,const Sphere& sphere);

    //とりあえずModelPipelineをカリング
    enum class RootParameter {
        kCallingInformation,
        kAppend,
        kViewProjection,
        
        parameterNum
    };

    PipelineState modelCallingPipeline_;
    RootSignature modelCallingRootSignature_;
    AppendBuffer appendBuffer_;
    StructuredBuffer callingInformation_;
    
private:
    void CreatePipeline();

  /*  D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[ParticleManager::CommandSigunature::kCommandSigunatureCount] = {};
    argumentDescs[ParticleManager::CommandSigunature::kParticleSRV].Type = D3D12_INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW;
    argumentDescs[ParticleManager::CommandSigunature::kParticleSRV].ShaderResourceView.RootParameterIndex = 0;
    argumentDescs[ParticleManager::CommandSigunature::kDrawIndexSRV].Type = D3D12_INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW;
    argumentDescs[ParticleManager::CommandSigunature::kDrawIndexSRV].ShaderResourceView.RootParameterIndex = 1;
    argumentDescs[ParticleManager::CommandSigunature::kDrawIndexBuffer].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

    D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc{};
    commandSignatureDesc.pArgumentDescs = argumentDescs;
    commandSignatureDesc.NumArgumentDescs = _countof(argumentDescs);
    commandSignatureDesc.ByteStride = sizeof(GPUParticleShaderStructs::IndirectCommand);
    auto result = device->CreateCommandSignature(&commandSignatureDesc, *graphicsRootSignature_, IID_PPV_ARGS(&commandSignature_));
    commandSignature_->SetName(L"commandSignature");
    assert(SUCCEEDED(result));*/

};

