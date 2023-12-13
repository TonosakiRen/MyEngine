#pragma once

#include <array>
#include "externals/DirectXTex/d3dx12.h"
#include <string>
#include <unordered_map>
#include <wrl.h>
#include <vector>
#include "Mesh.h"
#include "DescriptorHandle.h"

class ModelManager
{
public:
	static const size_t kNumModels = 256;

	struct ModelIndex {
		std::string name;
		std::vector<Mesh> meshes;
		uint32_t meshNum = 0;
	};


	static uint32_t Load(const std::string& fileName);

	static std::vector<Mesh> CreateMeshes(const std::string& fileName);

	static ModelManager* GetInstance();

	void DrawInstanced(ID3D12GraphicsCommandList* commandList,uint32_t modelHandle,UINT textureRootParamterIndex);

private:
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

	std::array<ModelIndex, kNumModels> models_;
	uint32_t useModelCount_ = 0;

	uint32_t LoadInternal(const std::string& fileName);

};

