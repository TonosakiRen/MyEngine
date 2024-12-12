#pragma once
#include <span>
#include <array>
#include <unordered_map>
#include <vector>
#include <optional>
#include <string>

#include "Mymath.h"
#include "GameComponent/WorldTransform.h"

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

class Skeleton {
public:

	void Create(const Node& rootNode);

	const Joint& GetJoint(std::string name) {
		int32_t index = 0;
		if (jointMap_.find(name) != jointMap_.end()) {
			index = jointMap_[name];
		}
		return joints_[index];
	}

	std::vector<Joint>& GetAllJoint() {
		return joints_;
	}

	void Update();

	void SetParent(WorldTransform* parent) {
		parent_ = parent;
	};

	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	int32_t root_; //RootJointのIndex
	std::unordered_map<std::string, int32_t> jointMap_; // Joint迷とIndexとの辞書
	std::vector<Joint> joints_; // 所属しているジョイント
	std::vector<WorldTransform> jointWordTransform_;
	WorldTransform* parent_;
};