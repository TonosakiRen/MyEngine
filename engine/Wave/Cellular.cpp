/**
 * @file Cellular.cpp
 * @brief CellularノイズのPipeline
 */
#include "Wave/Cellular.h"
#include "Light/ShaderManager.h"
#include "Graphics/CommandContext.h"
#include <vector>

#include "Graphics/Helper.h"

using namespace Microsoft::WRL;

Cellular::Cellular()
{
}

void Cellular::Initialize(uint32_t tileWidthNum)
{
    CreatePipeline();

    uint32_t width = 1024;

    cellularTexture_ = std::make_unique<ColorBuffer>();
    cellularTexture_->Create(L"cellularTexture",1024, 1024, DXGI_FORMAT_R32_FLOAT);

    tileWidthNum_ = tileWidthNum;
    tileWidth_ = float(width / tileWidthNum_);

    maxDistance_ = tileWidth_ * 2.0f;

    uint32_t pointNum = uint32_t(tileWidthNum_ * tileWidthNum_);



    point_.Create(L"cellularPointBuffer", sizeof(Point), pointNum);

    std::vector<Point> randomPoint;
    randomPoint.reserve(pointNum);

    for (uint32_t y = 0; y < tileWidthNum_; y++) {
        for (uint32_t x = 0; x < tileWidthNum_; x++) {
            Point tmp;
            float rndX = Rand(0.0f, float(tileWidth_)) + tileWidth_ * x;
            float rndY = Rand(0.0f, float(tileWidth_)) + tileWidth_ * y;
            tmp.point = Vector2{ rndX,rndY };
            tmp.index = y * tileWidthNum_ +  x;
            randomPoint.emplace_back(tmp);
        }
    }

    point_.Copy(randomPoint.data(), sizeof(Point) * pointNum);
}

void Cellular::Render(CommandContext& commandContext)
{
    commandContext.TransitionResource(*cellularTexture_, D3D12_RESOURCE_STATE_RENDER_TARGET);

    commandContext.ClearColor(*cellularTexture_);
    commandContext.SetViewportAndScissorRect(0, 0, cellularTexture_->GetWidth(), cellularTexture_->GetHeight());

    commandContext.SetGraphicsRootSignature(*sRootSignature);

    commandContext.SetPipelineState(*sPipelineState);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    commandContext.SetDescriptorTable(0, point_.GetSRV());
    commandContext.SetConstants(1, maxDistance_,tileWidthNum_, tileWidth_);

    
    commandContext.SetRenderTarget(cellularTexture_->GetRTV());
    commandContext.DrawInstanced(3,1, 0, 0);
    
    commandContext.TransitionResource(*cellularTexture_, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
}

void Cellular::Finalize()
{
    cellularTexture_.reset();
}

void Cellular::CreatePipeline()
{
    ComPtr<IDxcBlob> vsBlob;
    ComPtr<IDxcBlob> psBlob;
    ComPtr<ID3DBlob> errorBlob;

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"PostEffectVS.hlsl", ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    psBlob = shaderManager->Compile(L"CellularPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    sRootSignature = std::make_unique<RootSignature>();
    sPipelineState = std::make_unique<PipelineState>();

    {
        // デスクリプタレンジ
        CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
        descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[2] = {};
        rootparams[0].InitAsDescriptorTable(1, &descRangeSRV);
        rootparams[1].InitAsConstants(3, 0);

        // スタティックサンプラー
        CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
            CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

        // ルートシグネチャの設定
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.pParameters = rootparams;
        rootSignatureDesc.NumParameters = _countof(rootparams);
        rootSignatureDesc.pStaticSamplers = &samplerDesc;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        sRootSignature->Create(L"cellularRootSingature", rootSignatureDesc);

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

        // 深度バッファのフォーマット
        gpipeline.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

        // 図形の形状設定（三角形）
        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        gpipeline.NumRenderTargets = 1;                            // 描画対象は1つ
        gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT; // 0～255指定のRGBA
        gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

        gpipeline.pRootSignature = *sRootSignature;

        // グラフィックスパイプラインの生成
        sPipelineState->Create(L"cellularPipeline", gpipeline);
    }
}