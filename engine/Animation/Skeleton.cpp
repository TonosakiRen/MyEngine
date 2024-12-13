/**
 * @file Skeleton.cpp
 * @brief Skeleton構造体
 */
#include "Animation/Skeleton.h"

#include "Graphics/Helper.h"


void Skeleton::Create(const Node& rootNode)
{
	
	root_ = CreateJoint(rootNode, {}, joints_);

	// 名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : joints_) {
		jointMap_.emplace(joint.name, joint.index);
	}
	
}

void Skeleton::Update()
{
	//すべてのJOintを更新。親が若いので通常ループで処理可能になっている
	for (Joint& joint : joints_) {
		joint.localMatrix = MakeAffineMatrix(joint.transform.scale_, joint.transform.quaternion_, joint.transform.translation_);
		if (joint.parent) {
			joint.skeletonSpaceMatrix = joint.localMatrix * joints_[*joint.parent].skeletonSpaceMatrix;
		}
		else {
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

int32_t Skeleton::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
	Joint joint{};
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = MakeIdentity4x4();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size()); //現在登録されてる数をIndexに
	joint.parent = parent;
	joints.push_back(joint);// SkeletonのJoint列に追加
	for (const Node& child : node.children) {
		//このjointを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	return joint.index;
}
