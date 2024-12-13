/**
 * @file Sprite.cpp
 * @brief Sprite描画用Pipeline
 */
#include "Texture/Sprite.h"
#include "Texture/TextureManager.h"
#include <cassert>
#include "Graphics/WinApp.h"
#include "Light/ShaderManager.h"
#include "Render/Renderer.h"

using namespace Microsoft::WRL;

void Sprite::Initialize() {
	ComPtr<IDxcBlob> vsBlob;    
	ComPtr<IDxcBlob> psBlob;    
	ComPtr<ID3DBlob> errorBlob; 

	auto shaderManager = ShaderManager::GetInstance();

	vsBlob = shaderManager->Compile(L"SpriteVS.hlsl", ShaderManager::kVertex);
	assert(vsBlob != nullptr);

	psBlob = shaderManager->Compile(L"SpritePS.hlsl", ShaderManager::kPixel);
	assert(psBlob != nullptr);

	rootSignature_ = std::make_unique<RootSignature>();
	pipelineState_ = std::make_unique<PipelineState>();

	{

		CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER rootparams[2] = {};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
			CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // s0 レジスタ
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

		// ルートシグネチャの設定
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.pParameters = rootparams;
		rootSignatureDesc.NumParameters = _countof(rootparams);
		rootSignatureDesc.pStaticSamplers = &samplerDesc;
		rootSignatureDesc.NumStaticSamplers = 1;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		rootSignature_->Create(L"spriteRootSignature", rootSignatureDesc);

	}

	{

		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	  {
	   "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	  {
	   "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());

		gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;


		D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;


		gpipeline.BlendState.RenderTarget[0] = blenddesc;


		gpipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();


		gpipeline.InputLayout.pInputElementDescs = inputLayout;
		gpipeline.InputLayout.NumElements = _countof(inputLayout);


		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		gpipeline.NumRenderTargets = 1;
		gpipeline.RTVFormats[int(Renderer::kColor)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor);
		gpipeline.SampleDesc.Count = 1;

		gpipeline.pRootSignature = *rootSignature_;

		// グラフィックスパイプラインの生成
		pipelineState_->Create(L"spritePipeline", gpipeline);

	}

	matProjection_ = MakeOrthograohicmatrix(0.0f, 0.0f, (float)WinApp::kWindowWidth, (float)WinApp::kWindowHeight,  0.0f, 1.0f);
}

void Sprite::Finalize()
{
	rootSignature_.reset();
	pipelineState_.reset();
}

void Sprite::PreDraw(CommandContext& commandContext) {

	commandContext.SetPipelineState(*pipelineState_);
	commandContext.SetGraphicsRootSignature(*rootSignature_);
	commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Sprite::Draw(CommandContext& commandContext,SpriteData& spriteData) {

	spriteData.TransferVertices();

	spriteData.matWorld_ = MakeIdentity4x4();
	spriteData.matWorld_ *= MakeRotateZMatrix(spriteData.rotation_);
	spriteData.matWorld_ *= MakeTranslateMatrix({ spriteData.position_.x, spriteData.position_.y, 0.0f });

	ConstBufferData mapData;

	mapData.color = spriteData.color_;
	mapData.mat = spriteData.matWorld_ * matProjection_;

	spriteData.constBuffer_.Copy(mapData);

	commandContext.SetVertexBuffer(0, 1, &spriteData.vbView_[1]);

	commandContext.SetConstantBuffer(0, spriteData.constBuffer_.GetGPUVirtualAddress());

	TextureManager::GetInstance()->SetDescriptorTable(commandContext, 1, spriteData.textureHandle_);
	
	commandContext.DrawInstanced(4, 1, 0, 0);
}