#include "DeferredRenderer.h"
#include <Windows.h>
#include "DirectXCommon.h"
#include "ShaderManager.h"
#include "Helper.h"
#include <assert.h>

using namespace Microsoft::WRL;

RootSignature DeferredRenderer::rootSignature_;
PipelineState DeferredRenderer::pipelineState_;

void DeferredRenderer::Initialize(ColorBuffer* originalTexture)
{
	originalTexture_ = originalTexture;
	CreatePipeline();
	resultTexture_.Create(originalTexture_->GetWidth(), originalTexture_->GetHeight(), originalTexture_->GetFormat());
	dispatchNum_ = originalTexture_->GetWidth() * originalTexture_->GetHeight();
}

void DeferredRenderer::Dispatch(CommandContext& commandContext,ColorBuffer& normalBuffer,DepthBuffer& depthBuffer)
{
	commandContext.TransitionResource(resultTexture_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	commandContext.TransitionResource(normalBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	commandContext.TransitionResource(depthBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	commandContext.TransitionResource(*originalTexture_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	commandContext.SetPipelineState(pipelineState_);
	commandContext.SetComputeRootSignature(rootSignature_);
	commandContext.Dispatch(dispatchNum_, 1, 1);

}

void DeferredRenderer::CreatePipeline()
{
	ComPtr<IDxcBlob> uavBlob;

	auto shaderManager = ShaderManager::GetInstance();
	uavBlob = shaderManager->Compile(L"DeferredRenderingCS.hlsl", ShaderManager::kCompute);
	assert(uavBlob != nullptr);

	CD3DX12_DESCRIPTOR_RANGE ranges[3]{};
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

	CD3DX12_ROOT_PARAMETER rootparams[4]{};
	rootparams[0].InitAsDescriptorTable(0, &ranges[0]);
	rootparams[1].InitAsDescriptorTable(0, &ranges[1]);
	rootparams[2].InitAsDescriptorTable(0, &ranges[2]);
	rootparams[3].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.pParameters = rootparams;
	rootSignatureDesc.NumParameters = _countof(rootparams);

	rootSignature_.Create(rootSignatureDesc);

	D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
	desc.pRootSignature = rootSignature_;
	desc.CS = CD3DX12_SHADER_BYTECODE(uavBlob->GetBufferPointer(), uavBlob->GetBufferSize());

	pipelineState_.Create(desc);

	
}
