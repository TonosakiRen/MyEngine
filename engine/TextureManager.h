#pragma once

#include <array>
#include "CommandContext.h"
#include <string>
#include <unordered_map>
#include <wrl.h>
#include "GPUResource.h"
#include "DescriptorHandle.h"
#include <memory>
#include "UploadBuffer.h"

class TextureManager
{
public:
	static const size_t kNumTextures = 256;

	struct Texture {
		GPUResource resource;
		UploadBuffer intermediateResource;
		DescriptorHandle srvHandle;
		std::string name;
	};

	static uint32_t Load(const std::string& fileName);

	static uint32_t LoadUv(const std::string& fileName, const std::string& filePass);

	static TextureManager* GetInstance();

	void Initialize(CommandContext& commandContext ,std::string directoryPath = "Resources/textures/");
	void Finalize();

	const D3D12_RESOURCE_DESC GetResoureDesc(const uint32_t textureHandle);

	void SetGraphicsRootDescriptorTable(CommandContext& commandList, const  UINT rootParamIndex, const  uint32_t textureHandle);

	DescriptorHandle GetSRV(const std::string& fileName);

private:
	CommandContext* commandContext_ = nullptr;
private:
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	ID3D12Device* device_;
	UINT sDescriptorHandleIncrementSize_ = 0u;
	std::string directoryPath_;
	std::unique_ptr<std::array<Texture, kNumTextures>> textures_;
	uint32_t useTextureCount_ = 0;

	uint32_t LoadInternal(const std::string& fileName);
	uint32_t LoadUvInternal(const std::string& fileName,const std::string& filePass);
	
};