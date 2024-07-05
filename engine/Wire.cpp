#include "Wire.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "Helper.h"

using namespace Microsoft::WRL;

std::vector<Vector3> Wire::vertices_;

void Wire::Initialize() {
    CreatePipeline();
    vertexBufferView_ = std::make_unique<D3D12_VERTEX_BUFFER_VIEW>();
    vertexBuffer_ = std::make_unique<UploadBuffer>();

    vertexBuffer_->Create(sizeof(Vector3) * kLineNum);

    // 頂点バッファビューの作成
    vertexBufferView_->BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
    vertexBufferView_->SizeInBytes = sizeof(Vector3) * kLineNum;
    vertexBufferView_->StrideInBytes = sizeof(Vector3);
}

void Wire::Finalize()
{
    
}

void Wire::AllDraw(CommandContext& commandContext, const ViewProjection& viewProjection)
{
    
    assert(vertices_.size() < kLineNum);


    commandContext.SetPipelineState(*pipelineState_);
    commandContext.SetGraphicsRootSignature(*rootSignature_);

    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);


    vertexBuffer_->Copy(vertices_.data(), sizeof(Vector3) * vertices_.size());
    
    

    // CBVをセット（ワールド行列）
    commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kViewProjection), viewProjection.GetGPUVirtualAddress());

    commandContext.SetVertexBuffer(0,1,vertexBufferView_.get());

    commandContext.DrawInstanced(UINT(vertices_.size()),1);

    vertices_.clear();
}

void Wire::CreatePipeline() {
    HRESULT result = S_FALSE;
    ComPtr<IDxcBlob> vsBlob;    
    ComPtr<IDxcBlob> psBlob;    

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"WireVS.hlsl",ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    psBlob = shaderManager->Compile(L"WirePS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    rootSignature_ = std::make_unique<RootSignature>();
    pipelineState_ = std::make_unique<PipelineState>();

    {

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::parameterNum)] = {};
        rootparams[int(RootParameter::kViewProjection)].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

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

        rootSignature_->Create(rootSignatureDesc);

    }

    {

        D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
          {
           "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
        };

        // グラフィックスパイプラインの流れを設定
        D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
        gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
        gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());

        // サンプルマスク
        gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
        // ラスタライザステート
        gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        //  デプスステンシルステート
        gpipeline.DepthStencilState = Helper::DepthStateDisabled;

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
        /*D3D12_RENDER_TARGET_BLEND_DESC blenddesc2{};
        blenddesc2.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        blenddesc2.BlendEnable = false;
        blenddesc2.BlendOp = D3D12_BLEND_OP_ADD;
        blenddesc2.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blenddesc2.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

        blenddesc2.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        blenddesc2.SrcBlendAlpha = D3D12_BLEND_ONE;
        blenddesc2.DestBlendAlpha = D3D12_BLEND_ZERO;*/

        // ブレンドステートの設定
        /*gpipeline.BlendState.IndependentBlendEnable = true;
        gpipeline.BlendState.RenderTarget[0] = blenddesc;
        gpipeline.BlendState.RenderTarget[1] = blenddesc;
        gpipeline.BlendState.RenderTarget[2] = blenddesc2;*/

        gpipeline.BlendState.RenderTarget[0] = blenddesc;

        // 頂点レイアウトの設定
        gpipeline.InputLayout.pInputElementDescs = inputLayout;
        gpipeline.InputLayout.NumElements = _countof(inputLayout);

        // 図形の形状設定（三角形）
        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

        gpipeline.NumRenderTargets = 1;
        gpipeline.RTVFormats[int(Renderer::kColor)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor);
        /*gpipeline.RTVFormats[int(Renderer::kNormal)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kNormal);
        gpipeline.RTVFormats[int(Renderer::kMaterial)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kMaterial);*/
        gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

        gpipeline.pRootSignature = *rootSignature_;

        // グラフィックスパイプラインの生成
        pipelineState_->Create(gpipeline);
    }
}

void Wire::Draw(const std::vector<Vector3>& vertices)
{
    vertices_.reserve(vertices_.size() + vertices.size());
    std::copy(vertices.begin(), vertices.end(), std::back_inserter(vertices_));
}

void Wire::Draw(const Vector3& start,const Vector3& end)
{
    vertices_.push_back(start);
    vertices_.push_back(end);
}

void Wire::Draw(const Skeleton& skeleton,const WorldTransform& worldTransform)
{
    for (const Joint& joint : skeleton.joints) {
        Vector3 point = MakeTranslation(joint.skeletonSpaceMatrix) * worldTransform.matWorld_;
        vertices_.push_back(point);
        if (joint.parent) {
            vertices_.push_back(MakeTranslation(skeleton.joints.at(joint.parent.value()).skeletonSpaceMatrix) * worldTransform.matWorld_);
        }
        else {
            vertices_.push_back(point);
        }
    }
}

void Wire::Draw(const Frustum& frustum)
{
    Draw(frustum.vertex[0], frustum.vertex[1]);
    Draw(frustum.vertex[0], frustum.vertex[2]);
    Draw(frustum.vertex[2], frustum.vertex[3]);
    Draw(frustum.vertex[3], frustum.vertex[1]);

    Draw(frustum.vertex[4], frustum.vertex[5]);
    Draw(frustum.vertex[4], frustum.vertex[6]);
    Draw(frustum.vertex[6], frustum.vertex[7]);
    Draw(frustum.vertex[7], frustum.vertex[5]);

    Draw(frustum.vertex[0], frustum.vertex[4]);
    Draw(frustum.vertex[1], frustum.vertex[5]);
    Draw(frustum.vertex[2], frustum.vertex[6]);
    Draw(frustum.vertex[3], frustum.vertex[7]);
}
