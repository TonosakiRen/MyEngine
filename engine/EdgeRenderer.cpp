#include "EdgeRenderer.h"
#include <Windows.h>
#include "DirectXCommon.h"
#include "ShaderManager.h"
#include "Helper.h"
#include <assert.h>
#include "Renderer.h"
#include "ViewProjection.h"
#include "DirectionalLights.h"
#include "ImGuiManager.h"

using namespace Microsoft::WRL;

void EdgeRenderer::Initialize(ColorBuffer* colorTexture,ColorBuffer* normalTexture, DepthBuffer* depthTexture)
{
	colorTexture_ = colorTexture;
	normalTexture_ = normalTexture;
	depthTexture_ = depthTexture;
	CreatePipeline();
}

void EdgeRenderer::Render(CommandContext& commandContext, ColorBuffer& tmpBuffer, ColorBuffer* originalTexture)
{

	commandContext.TransitionResource(*originalTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandContext.TransitionResource(*normalTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandContext.TransitionResource(*depthTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	//Edge描画
	commandContext.TransitionResource(tmpBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandContext.SetRenderTarget(tmpBuffer.GetRTV());
	commandContext.SetViewportAndScissorRect(0, 0, tmpBuffer.GetWidth(), tmpBuffer.GetHeight());

	commandContext.SetPipelineState(edgePipelineState_);
	commandContext.SetGraphicsRootSignature(edgeRootSignature_);
	commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kColorTexture), originalTexture->GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kNormalTexture), normalTexture_->GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kDepthTexture), depthTexture_->GetSRV());


#ifdef _DEBUG
	if (ImGui::BeginMenu("Edge")) {
		ImGui::DragFloat3("EdgeColor", &edgeColor_.x,0.01f,0.0f,1.0f);
		ImGui::DragFloat("normalThreshold", &normalThreshold_, 0.0001f, 0.0f);
		ImGui::DragFloat("depthThreshold", &depthThreshold_, 0.0001f, 0.0f);
		ImGui::EndMenu();
	}
#endif
	commandContext.SetConstants(static_cast<UINT>(RootParameter::kEdgeColor), edgeColor_.x, edgeColor_.y,edgeColor_.z);
	commandContext.SetConstants(static_cast<UINT>(RootParameter::kEdgeParam), normalThreshold_, depthThreshold_);
	// 描画コマンド
	commandContext.DrawInstanced(3, 1, 0, 0);

	commandContext.CopyBuffer(*originalTexture, tmpBuffer);
}

void EdgeRenderer::CreatePipeline()
{
	ComPtr<IDxcBlob> vsBlob;
	ComPtr<IDxcBlob> psBlob;

	auto shaderManager = ShaderManager::GetInstance();

	vsBlob = shaderManager->Compile(L"EdgeVS.hlsl", ShaderManager::kVertex);
	assert(vsBlob != nullptr);

	psBlob = shaderManager->Compile(L"EdgePS.hlsl", ShaderManager::kPixel);
	assert(psBlob != nullptr);

	{

		CD3DX12_DESCRIPTOR_RANGE ranges[3]{};
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
		ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

		CD3DX12_ROOT_PARAMETER rootParameters[(int)RootParameter::ParameterNum]{};
		rootParameters[(int)RootParameter::kColorTexture].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kColorTexture]);
		rootParameters[(int)RootParameter::kNormalTexture].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kNormalTexture]);
		rootParameters[(int)RootParameter::kDepthTexture].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kDepthTexture]);
		rootParameters[int(RootParameter::kEdgeColor)].InitAsConstants(3, 0);
		rootParameters[int(RootParameter::kEdgeParam)].InitAsConstants(2, 1);

		// スタティックサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
			CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

		// ルートシグネチャの設定
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.pParameters = rootParameters;
		rootSignatureDesc.NumParameters = _countof(rootParameters);
		rootSignatureDesc.pStaticSamplers = &samplerDesc;
		rootSignatureDesc.NumStaticSamplers = 1;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		edgeRootSignature_.Create(L"edgeRootSignature", rootSignatureDesc);

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

		
		gpipeline.BlendState = Helper::BlendAlpha;

		// 図形の形状設定（三角形）
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		gpipeline.NumRenderTargets = 1;                            // 描画対象は1つ
		gpipeline.RTVFormats[0] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor); // 0～255指定のRGBA
		gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

		gpipeline.pRootSignature = edgeRootSignature_;

		edgePipelineState_.Create(L"edgePipeline", gpipeline);

	}
}