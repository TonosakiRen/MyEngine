#include "SpotLightShadowMap.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "LightNumBuffer.h"
#include "ShadowSpotLights.h"

using namespace Microsoft::WRL;

bool SpotLightShadowMap::isDrawSpotLightShadowMap = false;
CommandContext* SpotLightShadowMap::commandContext_ = nullptr;
std::unique_ptr<RootSignature> SpotLightShadowMap::rootSignature_;
std::unique_ptr<PipelineState> SpotLightShadowMap::pipelineState_;
ShadowSpotLights* SpotLightShadowMap::shadowSpotLights_;
void SpotLightShadowMap::StaticInitialize() {
    CreatePipeline();
}

void SpotLightShadowMap::Finalize()
{
    rootSignature_.reset();
    pipelineState_.reset();
}

void SpotLightShadowMap::PreDraw(CommandContext* commandContext, ShadowSpotLights& shadowSpotLights) {
    assert(SpotLightShadowMap::commandContext_ == nullptr);

    commandContext_ = commandContext;
    shadowSpotLights_ = &shadowSpotLights;
    isDrawSpotLightShadowMap = true;

    commandContext_->SetPipelineState(*pipelineState_);
    commandContext_->SetGraphicsRootSignature(*rootSignature_);

    for (int i = 0; i < ShadowSpotLights::lightNum; i++) {
        commandContext_->TransitionResource(shadowSpotLights_->lights_[i].shadowMap_, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        commandContext_->ClearDepth(shadowSpotLights_->lights_[i].shadowMap_);
    }

    commandContext_->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void SpotLightShadowMap::PostDraw() {
    commandContext_ = nullptr;
    isDrawSpotLightShadowMap = false;
}

void SpotLightShadowMap::CreatePipeline() {
    HRESULT result = S_FALSE;
    ComPtr<IDxcBlob> vsBlob;

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"SpotLightShadowMapVS.hlsl", ShaderManager::kVertex);
    assert(vsBlob != nullptr);


    rootSignature_ = std::make_unique<RootSignature>();
    pipelineState_ = std::make_unique<PipelineState>();

    {

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::parameterNum)] = {};
        rootparams[int(RootParameter::kWorldTransform)].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[(int)RootParameter::kShadowSpotLight].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

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


        D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
          {
           "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        };

        // グラフィックスパイプラインの流れを設定
        D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
        gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());

        // サンプルマスク
        gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
        // ラスタライザステート
        gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        //  デプスステンシルステート
        gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

        // 深度バッファのフォーマット
        gpipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();

        // 頂点レイアウトの設定
        gpipeline.InputLayout.pInputElementDescs = inputLayout;
        gpipeline.InputLayout.NumElements = _countof(inputLayout);

        // 図形の形状設定（三角形）
        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        gpipeline.SampleDesc.Count = 1;

        gpipeline.pRootSignature = *rootSignature_;

        // グラフィックスパイプラインの生成
        pipelineState_->Create(gpipeline);
    }
}

void SpotLightShadowMap::Draw(uint32_t modelHandle, const WorldTransform& worldTransform) {

    // CBVをセット（ワールド行列）
    commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

    for (int i = 0; i < ShadowSpotLights::lightNum; i++) {
        commandContext_->SetDepthStencil(shadowSpotLights_->lights_[i].shadowMap_.GetDSV());
        commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kShadowSpotLight), shadowSpotLights_->lights_[i].constBuffer_.GetGPUVirtualAddress());
        ModelManager::GetInstance()->DrawInstanced(commandContext_, modelHandle);
    }
}

