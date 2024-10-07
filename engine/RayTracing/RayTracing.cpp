#include "RayTracing.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ShaderManager.h"
#include "LightManager.h"

using namespace Microsoft::WRL;

void RayTracing::Initialize() {
    
}

void RayTracing::Finalize()
{
   
   rootSignature_.reset();
   pipelineState_.reset();
    
}

void RayTracing::CreatePipeline() {
    HRESULT result = S_FALSE;
    ComPtr<IDxcBlob> msBlob;
    ComPtr<IDxcBlob> psBlob;
    ComPtr<IDxcBlob> asBlob;

    auto shaderManager = ShaderManager::GetInstance();

    asBlob = shaderManager->Compile(L"MeshTestAS.hlsl", ShaderManager::kAmplification);
    assert(asBlob != nullptr);

    msBlob = shaderManager->Compile(L"MeshTestMS.hlsl", ShaderManager::kMesh);
    assert(msBlob != nullptr);

    psBlob = shaderManager->Compile(L"MeshTestPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    rootSignature_ = std::make_unique<RootSignature>();
    pipelineState_ = std::make_unique<PipelineState>();

    {
        // デスクリプタレンジ
        CD3DX12_DESCRIPTOR_RANGE descRangeSRV[int(RootParameter::kDescriptorRangeNum)];
        descRangeSRV[int(RootParameter::kTexture)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::parameterNum)] = {};
        rootparams[int(RootParameter::kTexture)].InitAsDescriptorTable(1, &descRangeSRV[int(RootParameter::kTexture)], D3D12_SHADER_VISIBILITY_ALL);

        rootparams[int(RootParameter::kWorldTransform)].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        // スタティックサンプラー
        CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
            CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT);

        // ルートシグネチャの設定
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.pParameters = rootparams;
        rootSignatureDesc.NumParameters = _countof(rootparams);
        rootSignatureDesc.pStaticSamplers = &samplerDesc;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        rootSignature_->Create(L"rayTraceingRootSignature", rootSignatureDesc);

    }

    {
        D3DX12_MESH_SHADER_PIPELINE_STATE_DESC pipeline = {};
        pipeline.AS = CD3DX12_SHADER_BYTECODE(asBlob->GetBufferPointer(), asBlob->GetBufferSize());
        pipeline.MS = CD3DX12_SHADER_BYTECODE(msBlob->GetBufferPointer(), msBlob->GetBufferSize());
        pipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());

        // サンプルマスク
        pipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
        // ラスタライザステート
        pipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        //  デプスステンシルステート
        pipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

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
        pipeline.BlendState.IndependentBlendEnable = true;
        pipeline.BlendState.RenderTarget[0] = blenddesc;
        pipeline.BlendState.RenderTarget[1] = blenddesc;

        // 深度バッファのフォーマット
        pipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();


        // 図形の形状設定（三角形）
        pipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        pipeline.NumRenderTargets = Renderer::kFRenderTargetNum;
        pipeline.RTVFormats[int(Renderer::kColor)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor);
        pipeline.RTVFormats[int(Renderer::kNormal)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kNormal);
        pipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

        pipeline.pRootSignature = *rootSignature_;

        CD3DX12_PIPELINE_MESH_STATE_STREAM psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(pipeline);

        D3D12_PIPELINE_STATE_STREAM_DESC streamDesc = {};
        streamDesc.pPipelineStateSubobjectStream = &psoStream;
        streamDesc.SizeInBytes = sizeof(psoStream);

        // グラフィックスパイプラインの生成
        pipelineState_->Create(L"rayTracingPipeline", streamDesc);
    }
}
