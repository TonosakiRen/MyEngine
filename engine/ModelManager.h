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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Node {
	WorldTransform transform;
	Matrix4x4 localMatrix = MakeIdentity4x4();
	std::string name;
	std::vector<Node> children;
};

struct Joint {
	WorldTransform transform; //Trsnform情報
	Matrix4x4 localMatrix; // localMatrix
	Matrix4x4 skeletonSpaceMatrix; // skeletonSpaceでの変換行列
	std::string name; //名前
	std::vector<int32_t> children; // 子JoinntのIndex. いなければnull
	int32_t index; // 自身のIndex
	std::optional<int32_t> parent; // 親JointのIndex,いなければnull
};

struct Skeleton {
	int32_t root; //RootJointのIndex
	std::unordered_map<std::string, int32_t> jointMap; // Joint迷とIndexとの辞書
	std::vector<Joint> joints; // 所属しているジョイント
};

class ModelManager
{
public:
	static const size_t kNumModels = 256;

	struct ModelData {
		std::string name;
		std::vector<Mesh> meshes;
		Node rootNode;
		//model全体のsize
		Vector3 modelSize{};
		Vector3 modelCenter{};
		uint32_t meshNum = 0;
	};


	static uint32_t Load(const std::string& fileName);

	static void CreateMeshes(ModelData& modelIndex);

	static ModelManager* GetInstance();

	Vector3 GetModelSize(uint32_t modelHandle);

	Vector3 GetModelCenter (uint32_t modelHandle);

	Node& GetRootNode(uint32_t modelHandle);

	void DrawInstanced(CommandContext* commandContext, uint32_t modelHandle);
	void DrawInstanced(CommandContext* commandContext,uint32_t modelHandle,UINT textureRootParamterIndex);
	void DrawInstanced(CommandContext* commandContext, uint32_t modelHandle, UINT textureRootParamterIndex, DescriptorHandle descriptorHandle);
	void DrawInstanced(CommandContext* commandContext, uint32_t modelHandle, UINT textureRootParamterIndex,uint32_t textureHandle);
	void DrawInstancing(CommandContext* commandContext, uint32_t modelHandle, UINT instancingNum, UINT textureRootParamterIndex);

	static Node ReadNode(aiNode* node);
	static Skeleton CreateSkelton(const Node& rootNode);
	static int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joint);
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

