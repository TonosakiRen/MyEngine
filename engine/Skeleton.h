#pragma once
#include <span>
#include <array>
#include <unordered_map>
#include <vector>
#include <optional>
#include <string>

#include "Mymath.h"
#include "WorldTransform.h"

struct Node {
	WorldTransform transform;
	Matrix4x4 localMatrix = MakeIdentity4x4();
	std::string name;
	std::vector<Node> children;
};

struct Joint {

	WorldTransform transform; //Transform情報
	Matrix4x4 localMatrix; // localMatrix
	Matrix4x4 skeletonSpaceMatrix; // skeletonSpaceでの変換行列
	std::string name; //名前
	std::vector<int32_t> children; // 子JointのIndex. いなければnull
	int32_t index; // 自身のIndex
	std::optional<int32_t> parent; // 親JointのIndex,いなければnull
};

class Skeleton{
public:

	void Create(const Node& rootNode);
	void Update();
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	int32_t root; //RootJointのIndex
	std::unordered_map<std::string, int32_t> jointMap; // Joint迷とIndexとの辞書
	std::vector<Joint> joints; // 所属しているジョイント
};