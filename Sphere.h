#pragma once
#include "ShaderResourceManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <Windows.h>
#include <d3d12.h>
#include "externals/DirectXTex/d3dx12.h"
#include <vector>
#include <wrl.h>
#include "Mymath.h"
#include "DirectionalLight.h"
#include "Material.h"

class DirectXCommon;

class Sphere
{
public: // 列挙子
	/// <summary>
	/// ルートパラメータ番号
	/// </summary>
	enum RootParameter {
		kWorldTransform, // ワールド変換行列
		kViewProjection, // ビュープロジェクション変換行列
		kTexture,        // テクスチャ
		kDirectionalLight, // ライト
		kMaterial, // ライト
		kShadowMap, //シャドウマップ
		kShadowViewProjection,//シャドウコンスト
		kRootParameterNum
	};

public: // サブクラス
	// 頂点データ構造体
	struct VertexPosNormalUv {
		Vector3 pos;    // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;     // uv座標
	};

	enum RenderPass {
		DefaultPass,
		ShadowPass,

		NonePass
	};

public: // 静的メンバ関数
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	static void StaticInitialize(uint32_t shadowMap_width = 1024, uint32_t shadowMap_height = 1024);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="commandList">描画コマンドリスト</param>
	static void BeginPass(ID3D12GraphicsCommandList* commandList, RenderPass pass);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void EndPass();

	/// <summary>
	/// 3Dモデル生成
	/// </summary>
	/// <returns></returns>
	static Sphere* Create();

private: // 静的メンバ変数
	// デバイス
	static DirectXCommon* sDirectXCommon;
	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize;
	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList;
	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature;
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState;
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sShadowPipelineState;
	// ShadowMap用Resorce
	static Microsoft::WRL::ComPtr<ID3D12Resource> sShadowMapRenderTargetResource;
	static Microsoft::WRL::ComPtr<ID3D12Resource> sShadowMapDepthStencilResource;
	//shadowMapWidthHeight
	static uint32_t shadowMapWidth;
	static uint32_t shadowMapHeight;
	static RenderPass currentRenderPass;

private: // 静的メンバ関数
	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	static void InitializeGraphicsPipelines();
	static void InitializeRootSignature();
	static void InitializeDefaultGraphicsPipelines();
	static void InitializeShadowGraphicsPipelines();
	static void CreateShadowMapRenderTarget();
	static void CreateShadowMapDepthStencil();
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, const DirectionalLight& directionalLight, const Material& material, const ViewProjection& shadowViewProjection, uint32_t textureHadle = 0);
	void DrawShadow(const WorldTransform& worldTransform, const ViewProjection& viewProjection);

	/// <summary>
	/// メッシュデータ生成
	/// </summary>
	void CreateMesh();

	/// <summary>
	/// shadow
	/// </summary>


private: // メンバ変数
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices_;
	// 頂点インデックス配列
	std::vector<uint16_t> indices_;
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff_;

};

