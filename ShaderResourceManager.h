#pragma once

#include <array>
#include "externals/DirectXTex/d3dx12.h"
#include <string>
#include <unordered_map>
#include <wrl.h>

/// <summary>
/// テクスチャマネージャー
/// </summary>
class ShaderResourceManager
{
public:
	//デスクリプターの数
	static const size_t kNumTextures = 255;

	struct Texture {
		//テクスチャリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		//シェーダリソースビューのハンドル(CPU)
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
		// シェーダリソースビューのハンドル(GPU)
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
		// 名前
		std::string name;
	};

	enum DescriptorHeapLayout {
		ShadowMapSrv,
		TextureSrvStart,
		NumDescriptors = TextureSrvStart + kNumTextures
	};

	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <returns>テクスチャハンドル</returns>
	static uint32_t Load(const std::string& fileName);

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static ShaderResourceManager* GetInstance();

	/// <summary>
	/// システム初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(std::string directoryPath = "Resources/");

	/// <summary>
	/// 全テクスチャリセット
	/// </summary>
	void ResetAll();

	/// <summary>
	/// リソース情報取得
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <returns>リソース情報</returns>
	const D3D12_RESOURCE_DESC GetResoureDesc(uint32_t textureHandle);

	/// <summary>
	/// デスクリプタテーブルをセット
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="rootParamIndex">ルートパラメータ番号</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void SetGraphicsRootDescriptorTable(
		ID3D12GraphicsCommandList* commandList, UINT rootParamIndex, uint32_t textureHandle);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() {
		return descriptorHeap_;
	}

	
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetShadowMapCPUDescriptorHandle() {
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeap_->GetCPUDescriptorHandleForHeapStart(), ShadowMapSrv, sDescriptorHandleIncrementSize_);
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE GetShadowMapGPUDescriptorHandle() {
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(descriptorHeap_->GetGPUDescriptorHandleForHeapStart(), ShadowMapSrv, sDescriptorHandleIncrementSize_);
	}


private:
	ShaderResourceManager() = default;
	~ShaderResourceManager() = default;
	ShaderResourceManager(const ShaderResourceManager&) = delete;
	ShaderResourceManager& operator=(const ShaderResourceManager&) = delete;

	// デバイス
	ID3D12Device* device_;
	// デスクリプタサイズ
	UINT sDescriptorHandleIncrementSize_ = 0u;
	// ディレクトリパス
	std::string directoryPath_;
	// デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	// 次に使うデスクリプタヒープの番号
	uint32_t indexNextDescriptorHeap_ = 0u;
	// テクスチャコンテナ
	std::array<Texture, kNumTextures> textures_;

	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	uint32_t LoadInternal(const std::string& fileName);
};

