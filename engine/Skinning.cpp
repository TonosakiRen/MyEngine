#include "Skinning.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ShaderManager.h"
#include "ImGuiManager.h"
#include "Renderer.h"
#include "Model.h"

#include <array>

using namespace Microsoft::WRL;

void Skinning::Initialize() {
    CreatePipeline();
}

void Skinning::Finalize()
{
    rootSignature_.reset();
    pipelineState_.reset();
}

void Skinning::PreDraw(CommandContext& commandContext, const ViewProjection& viewProjection) {

    commandContext.SetPipelineState(*pipelineState_);
    commandContext.SetGraphicsRootSignature(*rootSignature_);

    // CBVをセット（ビュープロジェクション行列）
    commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kViewProjection), viewProjection.GetGPUVirtualAddress());

    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Skinning::CreatePipeline() {
    HRESULT result = S_FALSE;
    ComPtr<IDxcBlob> vsBlob;    
    ComPtr<IDxcBlob> psBlob;    

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"SkinningVS.hlsl",ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    psBlob = shaderManager->Compile(L"ModelPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    rootSignature_ = std::make_unique<RootSignature>();
    pipelineState_ = std::make_unique<PipelineState>();

    {

        CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
        descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0

        CD3DX12_DESCRIPTOR_RANGE descRangeSRVMatrixPalette;
        descRangeSRVMatrixPalette.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::parameterNum)] = {};
        rootparams[int(RootParameter::kWorldTransform)].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kViewProjection)].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kTexture)].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kMaterial)].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[int(RootParameter::kMatrixPalette)].InitAsDescriptorTable(1, &descRangeSRVMatrixPalette, D3D12_SHADER_VISIBILITY_ALL);

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

        rootSignature_->Create(rootSignatureDesc);

    }

    {
       
        std::array<D3D12_INPUT_ELEMENT_DESC, 5>inputElementDescs{};
        inputElementDescs[0].SemanticName = "POSITION";
        inputElementDescs[0].SemanticIndex = 0;
        inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputElementDescs[0].InputSlot = 0;
        inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

        inputElementDescs[1].SemanticName = "NORMAL";
        inputElementDescs[1].SemanticIndex = 0;
        inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputElementDescs[1].InputSlot = 0;
        inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

        inputElementDescs[2].SemanticName = "TEXCOORD";
        inputElementDescs[2].SemanticIndex = 0;
        inputElementDescs[2].Format = DXGI_FORMAT_R32G32_FLOAT;
        inputElementDescs[2].InputSlot = 0;
        inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

        inputElementDescs[3].SemanticName = "WEIGHT";
        inputElementDescs[3].SemanticIndex = 0;
        inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        inputElementDescs[3].InputSlot = 1;
        inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

        inputElementDescs[4].SemanticName = "INDEX";
        inputElementDescs[4].SemanticIndex = 0;
        inputElementDescs[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
        inputElementDescs[4].InputSlot = 1;
        inputElementDescs[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

        // グラフィックスパイプラインの流れを設定
        D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
        gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
        gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());

        // サンプルマスク
        gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
        // ラスタライザステート
        gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        //  デプスステンシルステート
        gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

        // レンダーターゲットのブレンド設定
        D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
        blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        blenddesc.BlendEnable = true;
        blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
        blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

        blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
        blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

        // レンダーターゲットのブレンド設定
        D3D12_RENDER_TARGET_BLEND_DESC blenddesc2{};
        blenddesc2.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        blenddesc2.BlendEnable = false;
        blenddesc2.BlendOp = D3D12_BLEND_OP_ADD;
        blenddesc2.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blenddesc2.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

        blenddesc2.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        blenddesc2.SrcBlendAlpha = D3D12_BLEND_ONE;
        blenddesc2.DestBlendAlpha = D3D12_BLEND_ZERO;

        // ブレンドステートの設定
        gpipeline.BlendState.IndependentBlendEnable = true;
        gpipeline.BlendState.RenderTarget[0] = blenddesc;
        gpipeline.BlendState.RenderTarget[1] = blenddesc;
        gpipeline.BlendState.RenderTarget[2] = blenddesc2;

        // 深度バッファのフォーマット
        gpipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();

        // 頂点レイアウトの設定
        gpipeline.InputLayout.pInputElementDescs = inputElementDescs.data();
        gpipeline.InputLayout.NumElements = UINT(inputElementDescs.size());

        // 図形の形状設定（三角形）
        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        gpipeline.NumRenderTargets = Renderer::kRenderTargetNum;
        gpipeline.RTVFormats[int(Renderer::kColor)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor);
        gpipeline.RTVFormats[int(Renderer::kNormal)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kNormal);
        gpipeline.RTVFormats[int(Renderer::kMaterial)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kMaterial);
        gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

        gpipeline.pRootSignature = *rootSignature_;

        // グラフィックスパイプラインの生成
        pipelineState_->Create(gpipeline);
    }
}

void Skinning::Draw(CommandContext& commandContext,const uint32_t modelHandle, const WorldTransform& worldTransform, const Material& material, const SkinCluster& skinCluster) {

    // CBVをセット（ワールド行列）// CBVをセット（ワールド行列）
    commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

    // CBVをセット（マテリアル）
    commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kMaterial), material.GetGPUVirtualAddress());

    commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kMatrixPalette), skinCluster.GetSRV());

    ModelManager::GetInstance()->DrawSkinningInstanced(commandContext, modelHandle, skinCluster, static_cast<UINT>(RootParameter::kTexture));
}
