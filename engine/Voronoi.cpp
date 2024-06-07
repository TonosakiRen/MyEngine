#include "Voronoi.h"
#include "ShaderManager.h"
#include "CommandContext.h"
#include <vector>

#include "Helper.h"

using namespace Microsoft::WRL;

Voronoi::Voronoi()
{
}

void Voronoi::CreateMesh()
{

    vertices_.resize(4);

    //左下
    vertices_[0].pos = { -1.0f,-1.0f,0.0f,1.0f };
    vertices_[0].uv = { 0.0f,1.0f };
    //左上
    vertices_[1].pos = { -1.0f,1.0f,0.0f,1.0f };
    vertices_[1].uv = { 0.0f,0.0f };
    //右上
    vertices_[2].pos = { 1.0f,1.0f,0.0f,1.0f };
    vertices_[2].uv = { 1.0f,0.0f };
    //右下
    vertices_[3].pos = { 1.0f,-1.0f,0.0f,1.0f };
    vertices_[3].uv = { 1.0f,1.0f };

    // 頂点インデックスの設定
    indices_ = { 0,  1,  2, 0, 2, 3 };

    // 頂点データのサイズ
    UINT sizeVB = static_cast<UINT>(sizeof(VertexData) * vertices_.size());

    vertexBuffer_.Create(sizeVB);

    vertexBuffer_.Copy(vertices_.data(), sizeVB);

    // 頂点バッファビューの作成
    vbView_.BufferLocation = vertexBuffer_.GetGPUVirtualAddress();
    vbView_.SizeInBytes = sizeVB;
    vbView_.StrideInBytes = sizeof(vertices_[0]);


    // インデックスデータのサイズ
    UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices_.size());

    indexBuffer_.Create(sizeIB);

    indexBuffer_.Copy(indices_.data(), sizeIB);

    // インデックスバッファビューの作成
    ibView_.BufferLocation = indexBuffer_.GetGPUVirtualAddress();
    ibView_.Format = DXGI_FORMAT_R16_UINT;
    ibView_.SizeInBytes = sizeIB;

}

void Voronoi::Initialize(uint32_t pointNum)
{

    InitializeGraphicsPipeline();

    // メッシュ生成
    CreateMesh();

    voronoiTexture_.Create(1024, 1024, DXGI_FORMAT_R32G32B32A32_FLOAT);

    point_.Create(sizeof(Point), pointNum);

    std::vector<Point> randomPoint;
    randomPoint.reserve(pointNum);
    for (uint32_t i = 0; i < pointNum; i++) {
        float x = Rand(0.0f,float(voronoiTexture_.GetWidth()));
        float y = Rand(0.0f, float(voronoiTexture_.GetHeight()));

        Point tmp;
        tmp.point = Vector2{ x,y };

        //tmp.color = HSVA(Rand(0.0f,1.0f),1.0f,1.0f,1.0f);

        tmp.index = i;
        if (i % 350 == 0) {
            tmp.color = HSVA(Rand(0.0f, 1.0f), 0.5f, 1.0f, 1.0f);
        }
        else {
            tmp.color = { 0.0f,0.0f,0.0f,0.0f };
        }

        randomPoint.emplace_back(tmp);
    }

    pointNum_ = pointNum;

    point_.Copy(randomPoint.data(), sizeof(Point) * pointNum);
}

void Voronoi::Render(CommandContext& commandContext)
{
    commandContext.TransitionResource(voronoiTexture_, D3D12_RESOURCE_STATE_RENDER_TARGET);

    commandContext.ClearColor(voronoiTexture_);
    commandContext.SetViewportAndScissorRect(0, 0, voronoiTexture_.GetWidth(), voronoiTexture_.GetHeight());

    commandContext.SetGraphicsRootSignature(*sRootSignature);

    commandContext.SetVertexBuffer(0, vbView_);
    commandContext.SetIndexBuffer(ibView_);

    commandContext.SetPipelineState(*sPipelineState);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    commandContext.SetDescriptorTable(0, point_.GetSRV());
    commandContext.SetConstant(1, 0, pointNum_);

    
    commandContext.SetRenderTarget(voronoiTexture_.GetRTV());
    commandContext.DrawIndexed(static_cast<UINT>(indices_.size()), 0, 0);
    
    commandContext.TransitionResource(voronoiTexture_, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
}

void Voronoi::InitializeGraphicsPipeline()
{
    ComPtr<IDxcBlob> vsBlob;
    ComPtr<IDxcBlob> psBlob;
    ComPtr<ID3DBlob> errorBlob;

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"PostEffectVS.hlsl", ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    psBlob = shaderManager->Compile(L"VoronoiPS.hlsl", ShaderManager::kPixel);
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
        rootparams[1].InitAsConstants(1, 0);

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

        sRootSignature->Create(rootSignatureDesc);

    }

    {

        D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        {
         "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {
         "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        };

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

        // 頂点レイアウトの設定
        gpipeline.InputLayout.pInputElementDescs = inputLayout;
        gpipeline.InputLayout.NumElements = _countof(inputLayout);

        // 図形の形状設定（三角形）
        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        gpipeline.NumRenderTargets = 1;                            // 描画対象は1つ
        gpipeline.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT; // 0～255指定のRGBA
        gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

        gpipeline.pRootSignature = *sRootSignature;

        // グラフィックスパイプラインの生成
        sPipelineState->Create(gpipeline);
    }
}