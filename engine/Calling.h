#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "ModelManager.h"

class Calling
{
public:
	void Initialize();
	void SetViewProjection(const ViewProjection* viewProjection) {
		currentViewProjection = viewProjection;
	}

	ModelManager* modelManager = nullptr;
	const ViewProjection* currentViewProjection = nullptr;

	bool isDraw(const uint32_t modelHandle,const WorldTransform& worldTransform);
	uint32_t GetTileIndex();

	bool isFrustumSphereCollision(const Frustum& frustum,const Sphere& sphere);

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

