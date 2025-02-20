/**
 * @file TextureManager.cpp
 * @brief Textureリソースを管理する
 */
#include "Texture/TextureManager.h"
#include <DirectXTex.h>
#include <cassert>
#include "Graphics/DirectXCommon.h"
#include "GPUResource/UploadBuffer.h"
#include "GPUResource/BufferManager.h"
using namespace DirectX;

namespace Engine{

	uint32_t TextureManager::Load(const std::string& fileName) {
		return TextureManager::GetInstance()->LoadInternal(fileName);
	}

	uint32_t TextureManager::LoadUv(const std::string& fileName, const std::string& filePass) {
		return TextureManager::GetInstance()->LoadUvInternal(fileName, filePass);
	}

	TextureManager* TextureManager::GetInstance() {
		static TextureManager instance;
		if (!instance.textures_) {
			instance.textures_ = std::make_unique<std::array<Texture, kNumTextures>>();
		}
		return &instance;
	}

	DescriptorHandle TextureManager::GetSRV(const std::string& fileName) {
		return (*textures_)[TextureManager::GetInstance()->LoadInternal(fileName)].srvHandle;
	}

	void TextureManager::Initialize(CommandContext& commandContext, std::string directoryPath) {


		device_ = DirectXCommon::GetInstance()->GetDevice();
		directoryPath_ = directoryPath;
		commandContext_ = &commandContext;
		// デスクリプタサイズを取得
		sDescriptorHandleIncrementSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	}

	void TextureManager::Finalize() {

		textures_.reset();

	}

	const D3D12_RESOURCE_DESC TextureManager::GetResoureDesc(const uint32_t textureHandle) {
		assert(textureHandle < kNumTextures);
		return (*textures_)[textureHandle].resource->GetDesc();
	}

	void TextureManager::SetDescriptorTable(CommandContext& commandContext, const  UINT rootParamIndex, const uint32_t textureHandle) { // デスクリプタヒープの配列
		assert(textureHandle < kNumTextures);
		// シェーダリソースビューをセット
		commandContext.SetDescriptorTable(rootParamIndex, (*textures_)[textureHandle].srvHandle);
	}

	uint32_t TextureManager::LoadInternal(const std::string& fileName) {

		assert(useTextureCount_ < kNumTextures);
		uint32_t handle = useTextureCount_;

		// 読み込み済みテクスチャを検索
		auto it = std::find_if(textures_->begin(), textures_->end(), [&](const auto& texture) {return texture.name == fileName; });

		if (it != textures_->end()) {
			// 読み込み済みテクスチャの要素番号を取得
			handle = static_cast<uint32_t>(std::distance(textures_->begin(), it));
			return handle;
		}

		// 書き込むテクスチャの参照
		Texture& texture = textures_->at(useTextureCount_);
		texture.name = fileName;

		// ディレクトリパスとファイル名を連結してフルパスを得る
		bool currentRelative = false;
		if (2 < fileName.size()) {
			currentRelative = (fileName[0] == '.') && (fileName[1] == '/');
		}
		std::string fullPath = currentRelative ? fileName : directoryPath_ + fileName;

		// ユニコード文字列に変換
		wchar_t wfilePath[256];
		MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath, _countof(wfilePath));

		HRESULT result;

		TexMetadata metadata{};
		ScratchImage image{};

		// WICテクスチャのロード
		if (fullPath.ends_with(".dds")) {
			result = LoadFromDDSFile(wfilePath, DDS_FLAGS_NONE, nullptr, image);
		}
		else {
			result = LoadFromWICFile(wfilePath, WIC_FLAGS_NONE, nullptr, image);
		}
		assert(SUCCEEDED(result));

		ScratchImage mipImages{};

		metadata = image.GetMetadata();


		// ミップマップ生成
		if (IsCompressed(metadata.format)) {
			mipImages = std::move(image);
		}
		else {
			if (metadata.width != 1 && metadata.height != 1) {
				result = GenerateMipMaps(
					image.GetImages(), image.GetImageCount(), image.GetMetadata(),
					TEX_FILTER_SRGB, 0, mipImages);
			}
			else {
				mipImages = std::move(image);
			}
		}

		// metadata読み込んだディフューズテクスチャをSRGBとして扱う
		metadata = mipImages.GetMetadata();
		metadata.format = MakeSRGB(metadata.format);

		assert(SUCCEEDED(result));

		// リソース設定
		CD3DX12_RESOURCE_DESC texresDesc{};
		texresDesc.Width = UINT(metadata.width);
		texresDesc.Height = UINT(metadata.height);
		texresDesc.MipLevels = UINT16(metadata.mipLevels);
		texresDesc.DepthOrArraySize = UINT16(metadata.arraySize);
		texresDesc.Format = metadata.format;
		texresDesc.SampleDesc.Count = 1;
		texresDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

		texture.resource.CreateResource(L"textureResource", heapProps, texresDesc, D3D12_RESOURCE_STATE_COPY_DEST);

		// 中間リソースを読み込む
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		PrepareUpload(device_, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);


#ifdef _DEBUG
		texture.resource->SetName(L"Texture");
#endif // _DEBUG

		uint64_t intermediateSize = GetRequiredIntermediateSize(texture.resource, 0, UINT(subresources.size()));


		texture.intermediateResource.Create(L"intermediateResource", intermediateSize);

		UpdateSubresources(*commandContext_, texture.resource, texture.intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
		commandContext_->TransitionResource(texture.resource, D3D12_RESOURCE_STATE_GENERIC_READ);

		BufferManager::GetInstance()->ReleaseResource(texture.intermediateResource.GetIndex());

		// シェーダリソースビュー作成
		texture.srvHandle = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		D3D12_RESOURCE_DESC resDesc = texture.resource->GetDesc();

		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		if (metadata.IsCubemap()) {
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.MipLevels = UINT_MAX;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		}
		else {
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = (UINT)metadata.mipLevels;
		}

		device_->CreateShaderResourceView(
			texture.resource,
			&srvDesc,
			texture.srvHandle);

		useTextureCount_++;
		return handle;
	}

	uint32_t TextureManager::LoadUvInternal(const std::string& fileName, const std::string& filePass) {

		assert(useTextureCount_ < kNumTextures);
		uint32_t handle = useTextureCount_;

		// 読み込み済みテクスチャを検索
		auto it = std::find_if(textures_->begin(), textures_->end(), [&](const auto& texture) {return texture.name == fileName; });

		if (it != textures_->end()) {
			// 読み込み済みテクスチャの要素番号を取得
			handle = static_cast<uint32_t>(std::distance(textures_->begin(), it));
			return handle;
		}

		// 書き込むテクスチャの参照
		Texture& texture = textures_->at(useTextureCount_);
		texture.name = fileName;

		// ディレクトリパスとファイル名を連結してフルパスを得る
		bool currentRelative = false;
		if (2 < fileName.size()) {
			currentRelative = (fileName[0] == '.') && (fileName[1] == '/');
		}
		std::string fullPath = currentRelative ? fileName : filePass;

		// ユニコード文字列に変換
		wchar_t wfilePath[256];
		MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath, _countof(wfilePath));

		HRESULT result;

		TexMetadata metadata{};
		ScratchImage image{};

		// WICテクスチャのロード
		if (fullPath.ends_with(".dds")) {
			result = LoadFromDDSFile(wfilePath, DDS_FLAGS_NONE, nullptr, image);
		}
		else {
			result = LoadFromWICFile(wfilePath, WIC_FLAGS_NONE, nullptr, image);
		}
		assert(SUCCEEDED(result));

		ScratchImage mipImages{};

		metadata = image.GetMetadata();


		// ミップマップ生成
		if (IsCompressed(metadata.format)) {
			mipImages = std::move(image);
		}
		else {
			if (metadata.width != 1 && metadata.height != 1) {
				result = GenerateMipMaps(
					image.GetImages(), image.GetImageCount(), image.GetMetadata(),
					TEX_FILTER_SRGB, 0, mipImages);
			}
			else {
				mipImages = std::move(image);
			}
		}

		// metadata読み込んだディフューズテクスチャをSRGBとして扱う
		metadata = mipImages.GetMetadata();
		metadata.format = MakeSRGB(metadata.format);

		assert(SUCCEEDED(result));

		// リソース設定
		CD3DX12_RESOURCE_DESC texresDesc{};
		texresDesc.Width = UINT(metadata.width);
		texresDesc.Height = UINT(metadata.height);
		texresDesc.MipLevels = UINT16(metadata.mipLevels);
		texresDesc.DepthOrArraySize = UINT16(metadata.arraySize);
		texresDesc.Format = metadata.format;
		texresDesc.SampleDesc.Count = 1;
		texresDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

		texture.resource.CreateResource(L"uvTextureResource", heapProps, texresDesc, D3D12_RESOURCE_STATE_COPY_DEST);

		// 中間リソースを読み込む
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		PrepareUpload(device_, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);

		uint64_t intermediateSize = GetRequiredIntermediateSize(texture.resource, 0, UINT(subresources.size()));


		texture.intermediateResource.Create(L"uvIntermediaResource", intermediateSize);

		UpdateSubresources(*commandContext_, texture.resource, texture.intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
		commandContext_->TransitionResource(texture.resource, D3D12_RESOURCE_STATE_GENERIC_READ);

		BufferManager::GetInstance()->ReleaseResource(texture.intermediateResource.GetIndex());

		// シェーダリソースビュー作成
		texture.srvHandle = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		D3D12_RESOURCE_DESC resDesc = texture.resource->GetDesc();

		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		if (metadata.IsCubemap()) {
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.MipLevels = UINT_MAX;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		}
		else {
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = (UINT)metadata.mipLevels;
		}

		device_->CreateShaderResourceView(
			texture.resource,
			&srvDesc,
			texture.srvHandle);

		useTextureCount_++;
		return handle;
	}

}