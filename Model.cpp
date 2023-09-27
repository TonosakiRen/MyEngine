#include "Model.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <d3dcompiler.h>
#include "DirectXCommon.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
DirectXCommon* Model::sDirectXCommon = nullptr;
UINT Model::sDescriptorHandleIncrementSize = 0;
ID3D12GraphicsCommandList* Model::sCommandList = nullptr;
ComPtr<ID3D12RootSignature> Model::sRootSignature;
ComPtr<ID3D12PipelineState> Model::sPipelineState;
ComPtr<ID3D12PipelineState> Model::sShadowPipelineState;
ComPtr<ID3D12Resource> Model::sShadowMapRenderTargetResource;
ComPtr<ID3D12Resource> Model::sShadowMapDepthStencilResource;
uint32_t  Model::shadowMapWidth;
uint32_t  Model::shadowMapHeight;
Model::RenderPass Model::currentRenderPass = Model::NonePass;

void Model::StaticInitialize(uint32_t shadowMap_width, uint32_t shadowMap_height) {
	sDirectXCommon = DirectXCommon::GetInstance();
	shadowMapWidth = shadowMap_width;
	shadowMapHeight = shadowMap_height;

	// パイプライン初期化
	InitializeGraphicsPipelines();
}

void Model::BeginPass(ID3D12GraphicsCommandList* commandList, Model::RenderPass pass) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Model::sCommandList == nullptr);
	assert(currentRenderPass == Model::NonePass);

	// コマンドリストをセット
	sCommandList = commandList;

	// ルートシグネチャの設定
	commandList->SetGraphicsRootSignature(sRootSignature.Get());
	// プリミティブ形状を設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	switch (pass)
	{
	case Model::DefaultPass:
		// パイプラインステートの設定
		commandList->SetPipelineState(sPipelineState.Get());

		break;
	case Model::ShadowPass:

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = sShadowMapRenderTargetResource.Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		commandList->ResourceBarrier(1, &barrier);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = sDirectXCommon->GetShadowMapRtvHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = sDirectXCommon->GetShadowMapDsvHandle();
		commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
		float clearColor[4] = { 1.0f,1.0f,1.0f,1.0f };
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		D3D12_VIEWPORT viewport =
			CD3DX12_VIEWPORT(0.0f, 0.0f, float(shadowMapWidth), float(shadowMapHeight));
		commandList->RSSetViewports(1, &viewport);

		// シザリング矩形の設定
		CD3DX12_RECT rect = CD3DX12_RECT(0, 0, shadowMapWidth, shadowMapHeight);
		commandList->RSSetScissorRects(1, &rect);

		// パイプラインステートの設定
		commandList->SetPipelineState(sShadowPipelineState.Get());

		break;
	}
	currentRenderPass = pass;
}

void Model::EndPass() {
	assert(currentRenderPass != NonePass);

	if (currentRenderPass == ShadowPass) {
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = sShadowMapRenderTargetResource.Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		sCommandList->ResourceBarrier(1, &barrier);
	}

	// コマンドリストを解除
	sCommandList = nullptr;
	currentRenderPass = NonePass;
}

Model* Model::Create() {
	// 3Dオブジェクトのインスタンスを生成
	Model* object3d = new Model();
	assert(object3d);

	// 初期化
	object3d->Initialize();

	return object3d;
}

void Model::InitializeGraphicsPipelines() {
	InitializeRootSignature();
	InitializeDefaultGraphicsPipelines();
	InitializeShadowGraphicsPipelines();
	CreateShadowMapRenderTarget();
	CreateShadowMapDepthStencil();
}

void Model::CreateShadowMapRenderTarget() {
	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dtextureとして書き込む
	//RenderTargetTextureをライトのサイズで作成
	{
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = shadowMapWidth;//Textureの幅
		resourceDesc.Height = shadowMapHeight;//Textureの高さ
		resourceDesc.MipLevels = 1;//mipmapの数
		resourceDesc.DepthOrArraySize = 1;//奥行き or 配列Textureの配列数
		resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//RenderTargetとして利用可能なフォーマット
		resourceDesc.SampleDesc.Count = 1;//RenderTargetとして利用可能なフォーマット
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;//RenderTargetとして使う通知

		//利用するHeapの設定
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上に作る

		//色のクリア設定
		D3D12_CLEAR_VALUE colorClearValue{};
		colorClearValue.Color[0] = colorClearValue.Color[1] = colorClearValue.Color[2] = colorClearValue.Color[3] = 1.0f;//1.0f(最大値)でクリア
		colorClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//フォーマット。Resourceと合わせる

		//Resourceの生成
		ComPtr<ID3D12Resource> resource = nullptr;
		HRESULT hr = sDirectXCommon->GetDevice()->CreateCommittedResource(
			&heapProperties,//Heapの設定
			D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし。
			&resourceDesc, //Resoruceの設定
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,//ピクセルシェーダで使う状態にしておく
			&colorClearValue, //Clear最適値
			IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
		assert(SUCCEEDED(hr));

		sShadowMapRenderTargetResource = resource;

	}
	//ShadowMapRenderTargetを作る
	sDirectXCommon->GetDevice()->CreateRenderTargetView(sShadowMapRenderTargetResource.Get(), &rtvDesc, sDirectXCommon->GetShadowMapRtvHandle());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	sDirectXCommon->GetDevice()->CreateShaderResourceView(sShadowMapRenderTargetResource.Get(), &srvDesc, ShaderResourceManager::GetInstance()->GetCPUDescriptorHandle(ShaderResourceManager::DescriptorHeapLayout::ShadowMapSrv));
}

void Model::CreateShadowMapDepthStencil() {
	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format.基本的にはResourceに合わせる。
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
	//DepthStencilTextureをライトのサイズで作成
	{
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = shadowMapWidth;//Textureの幅
		resourceDesc.Height = shadowMapHeight;//Textureの高さ
		resourceDesc.MipLevels = 1;//mipmapの数
		resourceDesc.DepthOrArraySize = 1;//奥行き or 配列Textureの配列数
		resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DepthStencilとして利用可能なフォーマット
		resourceDesc.SampleDesc.Count = 1;//DepthStencilとして利用可能なフォーマット
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencilとして使う通知

		//利用するHeapの設定
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上に作る

		//深度値のクリア設定
		D3D12_CLEAR_VALUE depthClearValue{};
		depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリア
		depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//フォーマット。Resourceと合わせる

		//Resourceの生成
		ComPtr<ID3D12Resource> resource = nullptr;
		HRESULT hr = sDirectXCommon->GetDevice()->CreateCommittedResource(
			&heapProperties,//Heapの設定
			D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし。
			&resourceDesc, //Resoruceの設定
			D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値を書き込む状態にしておく
			&depthClearValue, //Clear最適値
			IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
		assert(SUCCEEDED(hr));

		sShadowMapDepthStencilResource = resource;

	}

	//ShadowMapDepthStencilResourceViewを作る
	sDirectXCommon->GetDevice()->CreateDepthStencilView(sShadowMapDepthStencilResource.Get(), &dsvDesc, sDirectXCommon->GetShadowMapDsvHandle());

}

void Model::InitializeDefaultGraphicsPipelines() {
	HRESULT result = S_FALSE;
	ComPtr<IDxcBlob> vsBlob;    // 頂点シェーダオブジェクト
	ComPtr<IDxcBlob> psBlob;    // ピクセルシェーダオブジェクト

	vsBlob = sDirectXCommon->CompileShader(L"BasicVS.hlsl", L"vs_6_0");
	assert(vsBlob != nullptr);

	psBlob = sDirectXCommon->CompileShader(L"BasicPS.hlsl", L"ps_6_0");
	assert(psBlob != nullptr);

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	  {// xy座標(1行で書いたほうが見やすい)
	   "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	  {// 法線ベクトル(1行で書いたほうが見やすい)
	   "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	  {// uv座標(1行で書いたほうが見やすい)
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
	// gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//  デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;                            // 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング


	gpipeline.pRootSignature = sRootSignature.Get();

	// グラフィックスパイプラインの生成
	result = sDirectXCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sPipelineState));
	assert(SUCCEEDED(result));
}
void Model::InitializeShadowGraphicsPipelines() {
	HRESULT result = S_FALSE;
	ComPtr<IDxcBlob> vsBlob;    // 頂点シェーダオブジェクト
	ComPtr<IDxcBlob> psBlob;    // ピクセルシェーダオブジェクト

	vsBlob = sDirectXCommon->CompileShader(L"ShadowVS.hlsl", L"vs_6_0");
	assert(vsBlob != nullptr);

	psBlob = sDirectXCommon->CompileShader(L"ShadowPS.hlsl", L"ps_6_0");
	assert(psBlob != nullptr);

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	  {// xy座標(1行で書いたほうが見やすい)
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
	// gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//  デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;                            // 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	gpipeline.pRootSignature = sRootSignature.Get();

	// グラフィックスパイプラインの生成
	result = sDirectXCommon->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sShadowPipelineState));
	assert(SUCCEEDED(result));
}
void Model::InitializeRootSignature() {
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト
	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ
	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV2;
	descRangeSRV2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[kRootParameterNum] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[4].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[5].InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[6].InitAsConstantBufferView(10, 0, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
		CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(
		_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ルートシグネチャの生成
	result = sDirectXCommon->GetDevice()->CreateRootSignature(
		0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&sRootSignature));
	assert(SUCCEEDED(result));
}


void Model::CreateMesh() {
	HRESULT result = S_FALSE;

	vertices_ = {
		//  x      y      z       nx     ny    nz       u     v
		// 前
		  {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // 左下
		  {{-1.0f, +1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 左上
		  {{+1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // 右下
		  {{+1.0f, +1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, // 右上
		  // 後(前面とZ座標の符号が逆)
		  {{+1.0f, -1.0f, +1.0f}, {0.0f, 0.0f, +1.0f}, {0.0f, 1.0f}}, // 左下
		  {{+1.0f, +1.0f, +1.0f}, {0.0f, 0.0f, +1.0f}, {0.0f, 0.0f}}, // 左上
		  {{-1.0f, -1.0f, +1.0f}, {0.0f, 0.0f, +1.0f}, {1.0f, 1.0f}}, // 右下
		  {{-1.0f, +1.0f, +1.0f}, {0.0f, 0.0f, +1.0f}, {1.0f, 0.0f}}, // 右上
		  // 左
		  {{-1.0f, -1.0f, +1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 左下
		  {{-1.0f, +1.0f, +1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 左上
		  {{-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 右下
		  {{-1.0f, +1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 右上
		  // 右（左面とX座標の符号が逆）
		  {{+1.0f, -1.0f, -1.0f}, {+1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 左下
		  {{+1.0f, +1.0f, -1.0f}, {+1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 左上
		  {{+1.0f, -1.0f, +1.0f}, {+1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 右下
		  {{+1.0f, +1.0f, +1.0f}, {+1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 右上
		  // 下
		  {{+1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // 左下
		  {{+1.0f, -1.0f, +1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // 左上
		  {{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // 右下
		  {{-1.0f, -1.0f, +1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // 右上
		  // 上（下面とY座標の符号が逆）
		  {{-1.0f, +1.0f, -1.0f}, {0.0f, +1.0f, 0.0f}, {0.0f, 1.0f}}, // 左下
		  {{-1.0f, +1.0f, +1.0f}, {0.0f, +1.0f, 0.0f}, {0.0f, 0.0f}}, // 左上
		  {{+1.0f, +1.0f, -1.0f}, {0.0f, +1.0f, 0.0f}, {1.0f, 1.0f}}, // 右下
		  {{+1.0f, +1.0f, +1.0f}, {0.0f, +1.0f, 0.0f}, {1.0f, 0.0f}}, // 右上
	};

	// 頂点インデックスの設定
	indices_ = { 0,  1,  3,  3,  2,  0,

				4,  5,  7,  7,  6,  4,

				8,  9,  11, 11, 10, 8,

				12, 13, 15, 15, 14, 12,

				16, 17, 19, 19, 18, 16,

				20, 21, 23, 23, 22, 20 };

	// 頂点データのサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices_.size());

	{
		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

		// 頂点バッファ生成
		result = sDirectXCommon->GetDevice()->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&vertBuff_));
		assert(SUCCEEDED(result));
	}

	// インデックスデータのサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices_.size());

	{
		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);

		// インデックスバッファ生成
		result = sDirectXCommon->GetDevice()->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&indexBuff_));
		assert(SUCCEEDED(result));
	}

	// 頂点バッファへのデータ転送
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}

	// インデックスバッファへのデータ転送
	uint16_t* indexMap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		std::copy(indices_.begin(), indices_.end(), indexMap);

		indexBuff_->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);

	// インデックスバッファビューの作成
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;
}

void Model::Initialize() {
	// nullptrチェック
	assert(sDirectXCommon->GetDevice());

	// メッシュ生成
	CreateMesh();
}

void Model::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, const DirectionalLight& directionalLight, const Material& material,const ViewProjection& shadowViewProjection,uint32_t textureHadle) {
	// nullptrチェック
	assert(sDirectXCommon->GetDevice());
	assert(sCommandList);
	assert(worldTransform.constBuff_.Get());
	assert(currentRenderPass != NonePass);

	// 頂点バッファの設定
	sCommandList->IASetVertexBuffers(0, 1, &vbView_);
	// インデックスバッファの設定
	sCommandList->IASetIndexBuffer(&ibView_);

	// CBVをセット（ワールド行列）
	sCommandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（ビュープロジェクション行列）
	sCommandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kViewProjection), viewProjection.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（シャドウビュープロジェクション行列）
	sCommandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kShadowViewProjection), shadowViewProjection.constBuff_->GetGPUVirtualAddress());

	if (currentRenderPass == DefaultPass) {
		// CBVをセット（ライト）
		sCommandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kDirectionalLight), directionalLight.constBuff_->GetGPUVirtualAddress());

		// CBVをセット（マテリアル）
		sCommandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kMaterial), material.constBuff_->GetGPUVirtualAddress());

		// SRVをセット
		ShaderResourceManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList, static_cast<UINT>(RootParameter::kTexture), textureHadle);

		//災厄の設計
		sCommandList->SetGraphicsRootDescriptorTable(static_cast<uint32_t>(RootParameter::kShadowMap), ShaderResourceManager::GetInstance()->GetGPUDescriptorHandle(ShaderResourceManager::ShadowMapSrv));
	}


	// 描画コマンド
	sCommandList->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
}

void Model::DrawShadow(const WorldTransform& worldTransform, const ViewProjection& viewProjection)
{
	// nullptrチェック
	assert(sDirectXCommon->GetDevice());
	assert(sCommandList);
	assert(worldTransform.constBuff_.Get());
	assert(currentRenderPass == ShadowPass);

	// 頂点バッファの設定
	sCommandList->IASetVertexBuffers(0, 1, &vbView_);
	// インデックスバッファの設定
	sCommandList->IASetIndexBuffer(&ibView_);

	// CBVをセット（ワールド行列）
	sCommandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（ビュープロジェクション行列）
	sCommandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kViewProjection), viewProjection.constBuff_->GetGPUVirtualAddress());

	// 描画コマンド
	sCommandList->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);
}
