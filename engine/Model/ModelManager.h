#pragma once
/**
 * @file ModelManager.h
 * @brief ModelをLoadや管理をする
 */
#include <array>
#include "Graphics/CommandContext.h"
#include <string>
#include <unordered_map>
#include <optional>
#include <wrl.h>
#include <vector>
#include "Mesh/Mesh.h"
#include "Graphics/DescriptorHandle.h"
#include "GPUResource/UploadBuffer.h"
#include "GameComponent/WorldTransform.h"
#include <memory>
#include <string>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Animation/Skeleton.h"
#include "Animation/SkinCluster.h"

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
	Sphere modelSphere{};
	uint32_t meshNum = 0;
};

class ModelManager
{
public:
	static const size_t kNumModels = 256;

	//Modelをロード
	static uint32_t Load(const std::string& fileName);

	static ModelManager* GetInstance();

	//Node読み込み
	static Node ReadNode(aiNode* node);

	void Initialize(CommandContext& commandContext_);
	void Finalize();
	//DrawCall
	void DrawInstanced(CommandContext& commandContext, const  uint32_t modelHandle);
	void DrawInstanced(CommandContext& commandContext, const uint32_t modelHandle, const UINT textureRootParamterIndex, const uint32_t textureHandle = 0);
	void DrawInstanced(CommandContext& commandContext, const  uint32_t modelHandle, UINT textureRootParamterIndex, DescriptorHandle descriptorHandle);
	void DrawInstanced(CommandContext& commandContext, const uint32_t modelHandle, SkinCluster& skincluster, const UINT textureRootParamterIndex, const uint32_t textureHandle = 0);
	//Instancing用DrawCall
	void DrawInstancing(CommandContext& commandContext, const  uint32_t modelHandle, UINT instancingNum, UINT textureRootParamterIndex);

	//Getter
	const Vector3& GetModelSize(const uint32_t modelHandle);
	const Vector3& GetModelCenter (const uint32_t modelHandle);
	const Sphere& GetModelSphere(const uint32_t modelHandle);
	Node& GetRootNode(const uint32_t modelHandle);
	ModelData& GetModelData(const uint32_t modelHandle);
	const std::vector<CullData>& GetCullDataData(const uint32_t modelHandle);
private:
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

	//Mesh生成
	void CreateMeshes(ModelData& modelIndex);
	//fileからロード
	uint32_t LoadInternal(const std::string& fileName);

	std::unique_ptr<std::array<ModelData, kNumModels>> models_;
	uint32_t useModelCount_ = 0;
	CommandContext* commandContext_ = nullptr;
	DirectXCommon* directXCommon_ = nullptr;

};

