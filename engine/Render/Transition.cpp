#include "Render/Transition.h"
#include <DirectXTex.h>
#include <d3dcompiler.h>
#include <fstream>
#include <sstream>
#include "Texture/TextureManager.h"
#include "Graphics/DWParam.h"
#include "Light/ShaderManager.h"
#include "Render/Renderer.h"

using namespace DirectX;
using namespace Microsoft::WRL;

void Transition::CreatePipeline() {
	ComPtr<IDxcBlob> vsBlob;
	ComPtr<IDxcBlob> psBlob;

	auto shaderManager = ShaderManager::GetInstance();

	vsBlob = shaderManager->Compile(L"PostEffectVS.hlsl", ShaderManager::kVertex);
	assert(vsBlob != nullptr);

	psBlob = shaderManager->Compile(L"TransitionPS.hlsl", ShaderManager::kPixel);
	assert(psBlob != nullptr);

	{

		// デスクリプタレンジ
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV[2];
		descRangeSRV[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ
		descRangeSRV[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t0 レジスタ

		// ルートパラメータ
		CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::ParameterNum)] = {};
		rootparams[int(RootParameter::kT)].InitAsConstants(1, 0);
		rootparams[int(RootParameter::kResult)].InitAsDescriptorTable(1, &descRangeSRV[0], D3D12_SHADER_VISIBILITY_ALL);
		rootparams[int(RootParameter::kTexture)].InitAsDescriptorTable(1, &descRangeSRV[1], D3D12_SHADER_VISIBILITY_ALL);

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

		rootSignature_.Create(L"transitionRootSignature", rootSignatureDesc);

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
		gpipeline.RTVFormats[0] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor); // 0～255指定のRGBA
		gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

		gpipeline.pRootSignature = rootSignature_;

		// グラフィックスパイプラインの生成
		pipelineState_.Create(L"transitionPipeline", gpipeline);
	}


}

void Transition::Initialize(ColorBuffer& resultBuffer) {
	CreatePipeline();
	saveResultBuffer_.Create(L"saveResultTexture", resultBuffer.GetWidth(), resultBuffer.GetHeight(), resultBuffer.GetFormat());
	isNextScene_ = false;
}

void Transition::Draw(ColorBuffer& resultBuffer, const DescriptorHandle& textureBuffer, CommandContext& commandContext) {

	if (isTransition_) {

		if (t_ >= 1.0f && isNextScene_ == false) {
			isNextScene_ = true;
		}

		if (t_ <= 0.0f && isNextScene_ == true) {
			isTransition_ = false;
			isNextScene_ = false;
			t_ = 0.0f;
		}
		else {
			if (!isNextScene_) {
				t_ += speed_;
				t_ = clamp(t_, 0.0f, 1.0f);
			}
			else {
				t_ -= speed_;
				t_ = clamp(t_, 0.0f, 1.0f);
			}
		}



		commandContext.CopyBuffer(saveResultBuffer_, resultBuffer);


		commandContext.TransitionResource(resultBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);

		commandContext.SetPipelineState(pipelineState_);
		commandContext.SetGraphicsRootSignature(rootSignature_);
		commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// CBVをセット（ワールド行列）
		commandContext.SetConstant(static_cast<UINT>(RootParameter::kT), 0, t_);

		// SRVをセット
		commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kTexture), textureBuffer);

		commandContext.TransitionResource(saveResultBuffer_, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
		commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kResult), saveResultBuffer_.GetSRV());

		// 描画コマンド
		commandContext.DrawInstanced(3,1,0,0);
	}

}