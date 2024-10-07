#include "DeferredRenderer.h"
#include <Windows.h>
#include "DirectXCommon.h"
#include "ShaderManager.h"
#include "Helper.h"
#include <assert.h>
#include "Renderer.h"
#include "ViewProjection.h"
#include "TileBasedRendering.h"
#include "LightManager.h"

using namespace Microsoft::WRL;

void DeferredRenderer::Initialize(ColorBuffer* colorTexture, ColorBuffer* normalTexture, ColorBuffer* materialTexture, DepthBuffer* depthTexture)
{
	normalTexture_ = normalTexture;
	depthTexture_ = depthTexture;
	colorTexture_ = colorTexture;
	materialTexture_ = materialTexture;
	CreatePipeline();
}

void DeferredRenderer::Render(CommandContext& commandContext, ColorBuffer* originalBuffer, ViewProjection& viewProjection, TileBasedRendering& tileBasedRendering)
{

	LightManager* lightManager = LightManager::GetInstance();

	commandContext.TransitionResource(*originalBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandContext.SetRenderTarget(originalBuffer->GetRTV());
	commandContext.SetViewportAndScissorRect(0, 0, originalBuffer->GetWidth(), originalBuffer->GetHeight());

	commandContext.TransitionResource(*colorTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandContext.TransitionResource(*normalTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandContext.TransitionResource(*materialTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandContext.TransitionResource(*depthTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	commandContext.SetPipelineState(pipelineState_);
	commandContext.SetGraphicsRootSignature(rootSignature_);
	commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kColorTexture), colorTexture_->GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kNormalTexture), normalTexture_->GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kMaterialTexture), materialTexture_->GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kDepthTexture), depthTexture_->GetSRV());

	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kDirectionalLights), lightManager->directionalLights_->structureBuffer_.GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kPointLights), lightManager->pointLights_->structureBuffer_.GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kAreaLights), lightManager->areaLights_->structureBuffer_.GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kSpotLights), lightManager->spotLights_->structureBuffer_.GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kShadowSpotLights), lightManager->shadowSpotLights_->structureBuffer_.GetSRV());

	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::k2DTextures), DirectXCommon::GetInstance()->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).GetDiscriptorStartHandle());

	commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kViewProjection), viewProjection.GetGPUVirtualAddress());
	commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kLightNum), lightManager->lightNumBuffer_->GetGPUVirtualAddress());


	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kTBRInformation), tileBasedRendering.GetTileInformationGPUHandle());
	commandContext.SetConstants(static_cast<UINT>(RootParameter::kTileNum), TileBasedRendering::kTileWidthNum, TileBasedRendering::kTileHeightNum);

	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kTBRPointLightIndex), tileBasedRendering.GetPointLightIndexGPUHandle());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kTBRSpotLightIndex), tileBasedRendering.GetSpotLightIndexGPUHandle());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kTBRShadowSpotLightIndex), tileBasedRendering.GetShadowSpotLightIndexGPUHandle());

	commandContext.DrawInstanced(3, 1, 0, 0);
}

void DeferredRenderer::CreatePipeline()
{
	ComPtr<IDxcBlob> vsBlob;
	ComPtr<IDxcBlob> psBlob;

	auto shaderManager = ShaderManager::GetInstance();

	vsBlob = shaderManager->Compile(L"PostEffectVS.hlsl", ShaderManager::kVertex);
	assert(vsBlob != nullptr);

	psBlob = shaderManager->Compile(L"DeferredRenderingPS.hlsl", ShaderManager::kPixel);
	assert(psBlob != nullptr);

	{

		CD3DX12_DESCRIPTOR_RANGE ranges[int(RootParameter::kDescriptorRageNum)]{};
		ranges[int(RootParameter::kColorTexture)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		ranges[int(RootParameter::kNormalTexture)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
		ranges[int(RootParameter::kMaterialTexture)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
		ranges[int(RootParameter::kDepthTexture)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);

		ranges[int(RootParameter::kDirectionalLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);
		ranges[int(RootParameter::kPointLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);
		ranges[int(RootParameter::kAreaLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6);
		ranges[int(RootParameter::kSpotLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 7);
		ranges[int(RootParameter::kShadowSpotLights)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 8);

		ranges[int(RootParameter::k2DTextures)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, DirectXCommon::GetInstance()->DirectXCommon::kSrvHeapDescriptorNum, 0, 1);

		ranges[int(RootParameter::kTBRInformation)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

		ranges[int(RootParameter::kTBRPointLightIndex)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
		ranges[int(RootParameter::kTBRSpotLightIndex)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2);
		ranges[int(RootParameter::kTBRShadowSpotLightIndex)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 3);

		CD3DX12_ROOT_PARAMETER rootParameters[(int)RootParameter::ParameterNum]{};
		rootParameters[(int)RootParameter::kColorTexture].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kColorTexture]);
		rootParameters[(int)RootParameter::kNormalTexture].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kNormalTexture]);
		rootParameters[(int)RootParameter::kMaterialTexture].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kMaterialTexture]);
		rootParameters[(int)RootParameter::kDepthTexture].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kDepthTexture]);

		rootParameters[(int)RootParameter::kDirectionalLights].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kDirectionalLights]);
		rootParameters[(int)RootParameter::kPointLights].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kPointLights]);
		rootParameters[(int)RootParameter::kAreaLights].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kAreaLights]);
		rootParameters[(int)RootParameter::kSpotLights].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kSpotLights]);
		rootParameters[(int)RootParameter::kShadowSpotLights].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kShadowSpotLights]);

		rootParameters[(int)RootParameter::k2DTextures].InitAsDescriptorTable(1, &ranges[(int)RootParameter::k2DTextures]);


		rootParameters[(int)RootParameter::kTBRInformation].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kTBRInformation]);

		rootParameters[(int)RootParameter::kTBRPointLightIndex].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kTBRPointLightIndex]);
		rootParameters[(int)RootParameter::kTBRSpotLightIndex].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kTBRSpotLightIndex]);
		rootParameters[(int)RootParameter::kTBRShadowSpotLightIndex].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kTBRShadowSpotLightIndex]);

		rootParameters[(int)RootParameter::kViewProjection].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[(int)RootParameter::kLightNum].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[(int)RootParameter::kTileNum].InitAsConstants(2, 2);

		// スタティックサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
			CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

		// ルートシグネチャの設定
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.pParameters = rootParameters;
		rootSignatureDesc.NumParameters = _countof(rootParameters);
		rootSignatureDesc.pStaticSamplers = &samplerDesc;
		rootSignatureDesc.NumStaticSamplers = 1;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		rootSignature_.Create(L"deferredRootSignature", rootSignatureDesc);

	}

	{

		// グラフィックスパイプラインの流れを設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());

		// サンプルマスク
		gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
		// ラスタライザステート
		gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

		// ブレンドステートの設定
		gpipeline.BlendState = Helper::BlendDisable;

		// 図形の形状設定（三角形）
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		gpipeline.NumRenderTargets = 1;                            // 描画対象は1つ
		gpipeline.RTVFormats[0] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor); // 0～255指定のRGBA
		gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

		gpipeline.pRootSignature = rootSignature_;

		pipelineState_.Create(L"deferredPipeline", gpipeline);

	}


}