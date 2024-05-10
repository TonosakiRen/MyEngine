#pragma once

#include <array>
#include "CommandContext.h"
#include <string>
#include <unordered_map>
#include <optional>
#include <wrl.h>
#include <vector>
#include "Mesh.h"
#include "DescriptorHandle.h"
#include "UploadBuffer.h"
#include "WorldTransform.h"
#include <memory>
#include <string>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Skeleton.h"
#include "SkinCluster.h"

struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};

struct JointWeightData {
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};

struct ModelData {
	std::string name;
	std::vector<Mesh> meshes;
	Node rootNode;
	std::map<std::string, JointWeightData> skinClusterData;
	//model全体のsize
	Vector3 modelSize{};
	Vector3 modelCenter{};
	uint32_t meshNum = 0;
};

class ModelManager
{
public:
	static const size_t kNumModels = 256;


	static uint32_t Load(const std::string& fileName);

	static void CreateMeshes(ModelData& modelIndex);

	static ModelManager* GetInstance();

	const Vector3& GetModelSize(uint32_t modelHandle);

	const Vector3& GetModelCenter (uint32_t modelHandle);

	Node& GetRootNode(uint32_t modelHandle);

	const ModelData& GetModelData(uint32_t modelHandle);

	void DrawInstanced(CommandContext* commandContext, uint32_t modelHandle);
	void DrawInstanced(CommandContext* commandContext,uint32_t modelHandle,UINT textureRootParamterIndex);
	void DrawInstanced(CommandContext* commandContext, uint32_t modelHandle, UINT textureRootParamterIndex, DescriptorHandle descriptorHandle);
	void DrawInstanced(CommandContext* commandContext, uint32_t modelHandle, UINT textureRootParamterIndex,uint32_t textureHandle);
	void DrawInstancing(CommandContext* commandContext, uint32_t modelHandle, UINT instancingNum, UINT textureRootParamterIndex);

	void DrawSkinningInstanced(CommandContext* commandContext, uint32_t modelHandle,const SkinCluster& skinCluster, UINT textureRootParamterIndex, uint32_t textureHandle = 0);

	static Node ReadNode(aiNode* node);

	void Finalize();
private:
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

	std::unique_ptr<std::array<ModelData, kNumModels>> models_;
	uint32_t useModelCount_ = 0;

	uint32_t LoadInternal(const std::string& fileName);

};

