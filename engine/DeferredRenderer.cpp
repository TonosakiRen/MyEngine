#include "DeferredRenderer.h"
#include <Windows.h>
#include "DirectXCommon.h"
#include "ShaderManager.h"
#include "Helper.h"
#include <assert.h>
#include "Renderer.h"
#include "ViewProjection.h"
#include "DirectionalLights.h"
#include "PointLights.h"
#include "AreaLights.h"
#include "SpotLights.h"
#include "LightNumBuffer.h"
#include "ShadowSpotLights.h"
#include "TileBasedRendering.h"

using namespace Microsoft::WRL;

void DeferredRenderer::Initialize(ColorBuffer* colorTexture, ColorBuffer* normalTexture, ColorBuffer* materialTexture, DepthBuffer* depthTexture)
{
	normalTexture_ = normalTexture;
	depthTexture_ = depthTexture;
	colorTexture_ = colorTexture;
	materialTexture_ = materialTexture;
	CreatePipeline();
}

void DeferredRenderer::Render(CommandContext& commandContext, ColorBuffer* originalBuffer,ViewProjection& viewProjection, DirectionalLights& directionalLight, PointLights& pointLights, AreaLights& areaLights ,SpotLights& spotLights, ShadowSpotLights& shadowSpotLights, LightNumBuffer& lightNumBuffer, TileBasedRendering& tileBasedRendering)
{
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

	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kDirectionalLights), directionalLight.srvHandle_);
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kPointLights), pointLights.structureBuffer_.GetSRV(commandContext));
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kAreaLights), areaLights.structureBuffer_.GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kSpotLights), spotLights.srvHandle_);
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kShadowSpotLights), shadowSpotLights.srvHandle_);

	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::k2DTextures), DirectXCommon::GetInstance()->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).GetDiscriptorStartHandle());

	commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kViewProjection), viewProjection.GetGPUVirtualAddress());
	commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kLightNum), lightNumBuffer.GetGPUVirtualAddress());

	commandContext.TransitionResource(tileBasedRendering.pointLightIndexBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandContext.TransitionResource(tileBasedRendering.tileInformationBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kTBRInformation), tileBasedRendering.tileInformationBuffer_.GetSRV());
	commandContext.SetConstants(static_cast<UINT>(RootParameter::kTileNum),TileBasedRendering::kTileWidthNum, TileBasedRendering::kTileHeightNum);

	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kTBRPointLightIndex), tileBasedRendering.pointLightIndexBuffer_.GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kTBRSpotLightIndex), tileBasedRendering.spotLightIndexBuffer_.GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kTBRShadowSpotLightIndex), tileBasedRendering.shadowSpotLightIndexBuffer_.GetSRV());

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

		CD3DX12_DESCRIPTOR_RANGE ranges[14]{};
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
		ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
		ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);

		ranges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);
		ranges[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);
		ranges[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6);
		ranges[7].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 7);
		ranges[8].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 8);

		ranges[9].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, DirectXCommon::GetInstance()->DirectXCommon::kSrvHeapDescriptorNum, 0, 1);

		ranges[10].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 9);

		ranges[11].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 10);
		ranges[12].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 11);
		ranges[13].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 12);

		CD3DX12_ROOT_PARAMETER rootParameters[(int)RootParameter::ParameterNum]{};
		rootParameters[(int)RootParameter::kColorTexture].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kColorTexture]);
		rootParameters[(int)RootParameter::kNormalTexture].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kNormalTexture]);
		rootParameters[(int)RootParameter::kMaterialTexture].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kMaterialTexture]);
		rootParameters[(int)RootParameter::kDepthTexture].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kDepthTexture]);
		rootParameters[(int)RootParameter::kViewProjection].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

		rootParameters[(int)RootParameter::kDirectionalLights].InitAsDescriptorTable(1, &ranges[4]);
		rootParameters[(int)RootParameter::kPointLights].InitAsDescriptorTable(1, &ranges[5]);
		rootParameters[(int)RootParameter::kAreaLights].InitAsDescriptorTable(1, &ranges[6]);
		rootParameters[(int)RootParameter::kSpotLights].InitAsDescriptorTable(1, &ranges[7]);
		rootParameters[(int)RootParameter::kShadowSpotLights].InitAsDescriptorTable(1, &ranges[8]);

		rootParameters[(int)RootParameter::k2DTextures].InitAsDescriptorTable(1, &ranges[9]);

		rootParameters[(int)RootParameter::kLightNum].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

		rootParameters[(int)RootParameter::kTBRInformation].InitAsDescriptorTable(1, &ranges[10]);
		rootParameters[(int)RootParameter::kTileNum].InitAsConstants(2, 2);

		rootParameters[(int)RootParameter::kTBRPointLightIndex].InitAsDescriptorTable(1, &ranges[11]);
		rootParameters[(int)RootParameter::kTBRSpotLightIndex].InitAsDescriptorTable(1, &ranges[12]);
		rootParameters[(int)RootParameter::kTBRShadowSpotLightIndex].InitAsDescriptorTable(1, &ranges[13]);


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

		rootSignature_.Create(rootSignatureDesc);

	}

	{

		/*D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			  {
				"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
			   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			  {
			   "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT,
			   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};*/

		// グラフィックスパイプラインの流れを設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());

		// サンプルマスク
		gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
		// ラスタライザステート
		gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

		// レンダーターゲットのブレンド設定
		D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blenddesc.BlendEnable = false;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		// ブレンドステートの設定
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		// 図形の形状設定（三角形）
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		gpipeline.NumRenderTargets = 1;                            // 描画対象は1つ
		gpipeline.RTVFormats[0] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor); // 0～255指定のRGBA
		gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

		gpipeline.pRootSignature = rootSignature_;

		pipelineState_.Create(gpipeline);

	}


}
