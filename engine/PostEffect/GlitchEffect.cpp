/**
 * @file GlitchEffect.cpp
 * @brief GlitchEffectのPipeline
 */
#include "PostEffect/GlitchEffect.h"
#include <DirectXTex.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include "Light/ShaderManager.h"
#include "Render/Renderer.h"
#include "ImGuiManager.h"
#include "Framework/Framework.h"

using namespace DirectX;
using namespace Microsoft::WRL;

void GlitchEffect::CreatePipeline(ColorBuffer& orignalBuffer) {
	ComPtr<IDxcBlob> vsBlob;
	ComPtr<IDxcBlob> psBlob;

	auto shaderManager = ShaderManager::GetInstance();

	vsBlob = shaderManager->Compile(L"PostEffectVS.hlsl", ShaderManager::kVertex);
	assert(vsBlob != nullptr);

	psBlob = shaderManager->Compile(L"GlitchEffectPS.hlsl", ShaderManager::kPixel);
	assert(psBlob != nullptr);

	{

		// デスクリプタレンジ
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

		// ルートパラメータ
		CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::ParameterNum)] = {};
		rootparams[int(RootParameter::kTexture)].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[int(RootParameter::kData)].InitAsConstants(4, 0);

		// スタティックサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
			CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

		// ルートシグネチャの設定
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.pParameters = rootparams;
		rootSignatureDesc.NumParameters = _countof(rootparams);
		rootSignatureDesc.pStaticSamplers = &samplerDesc;
		rootSignatureDesc.NumStaticSamplers = 1;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		rootSignature_.Create(L"glitchEffectRootSignature", rootSignatureDesc);

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
		gpipeline.RTVFormats[0] = orignalBuffer.GetFormat(); // 0～255指定のRGBA
		gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

		gpipeline.pRootSignature = rootSignature_;

		// グラフィックスパイプラインの生成
		pipelineState_.Create(L"glitchPipeline", gpipeline);
	}
}



void GlitchEffect::Initialize(ColorBuffer& originalBuffer) {
	CreatePipeline(originalBuffer);
}

void GlitchEffect::Draw(ColorBuffer& originalBuffer, ColorBuffer& tmpBuffer, CommandContext& commandContext) {

	commandContext.TransitionResource(originalBuffer, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	commandContext.TransitionResource(tmpBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);

	commandContext.SetRenderTarget(tmpBuffer.GetRTV());
	commandContext.SetPipelineState(pipelineState_);
	commandContext.SetGraphicsRootSignature(rootSignature_);
	commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// SRVをセット
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kTexture), originalBuffer.GetSRV());

#ifdef USE_IMGUI
	if (ImGui::BeginMenu("GlitchEffect")) {
		ImGui::DragFloat("intensity", &intensity_, 0.01f, 0.1f);
		ImGui::DragFloat("speed", &speed_, 0.01f, 0.1f);
		ImGui::DragFloat("scale", &scale_, 0.01f, 0.1f);
		ImGui::EndMenu();
	}
#endif

	commandContext.SetConstants(static_cast<UINT>(RootParameter::kData), Framework::kFrame,intensity_,speed_,scale_);
	

	// 描画コマンド
	commandContext.DrawInstanced(3, 1, 0, 0);

	commandContext.CopyBuffer(originalBuffer, tmpBuffer);
}

