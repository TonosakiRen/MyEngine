/**
 * @file SkinCluster.cpp
 * @brief Skinningをするためのモジュール
 */
#include "Animation/SkinCluster.h"

#include "Graphics/Helper.h"
#include "Graphics/DirectXCommon.h"
#include "Model/ModelManager.h"

void SkinCluster::Create(Skeleton& skeleton, const uint32_t modelHandle) {

    skeleton_ = &skeleton;
    const ModelData& modelData = ModelManager::GetInstance()->GetModelData(modelHandle);

    paletteResource_.Create(L"paletteResource", sizeof(WellForGPU), UINT(modelData.meshes[0].GetVerticies().size()));

    paletteResource_.SetZero();

    influenceResource_.Create(L"influenceResource", sizeof(VertexInfluence) * modelData.meshes[0].GetVerticies().size());
    influenceResource_.SetZero();

    influenceBufferView_.BufferLocation = influenceResource_->GetGPUVirtualAddress();
    influenceBufferView_.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.meshes[0].GetVerticies().size());
    influenceBufferView_.StrideInBytes = sizeof(VertexInfluence);

    inverseBindPoseMatrices_.resize(skeleton.GetAllJoint().size());
    std::generate(inverseBindPoseMatrices_.begin(), inverseBindPoseMatrices_.end(), MakeIdentity4x4);

    for (const auto& jointWeight : modelData.skinClusterData) {
        auto it = skeleton.GetJointMap().find(jointWeight.first);
        if (it == skeleton.GetJointMap().end()) {
            continue;
        }

        inverseBindPoseMatrices_[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
        for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
            VertexInfluence* vertexInfluence = static_cast<VertexInfluence*>(influenceResource_.GetCPUData());
            auto& currentInfluence = vertexInfluence[vertexWeight.vertexIndex];
            for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
                if (currentInfluence.weights[index] == 0.0f) {
                    currentInfluence.weights[index] = vertexWeight.weight;
                    currentInfluence.jointIndices[index] = (*it).second;
                    break;
                }
            }
        }
    }

    influences_.Create(L"vertexInfluence", sizeof(VertexInfluence), UINT(modelData.meshes[0].GetVerticies().size()));
    influences_.Copy(influenceResource_.GetCPUData(), UINT(sizeof(VertexInfluence) * modelData.meshes[0].GetVerticies().size()));

    skinnedVertices_.Create(L"skinnedVertices", sizeof(Mesh::VertexData), UINT(modelData.meshes[0].GetVerticies().size()));

    skinnedBufferView_.BufferLocation = skinnedVertices_->GetGPUVirtualAddress();
    skinnedBufferView_.SizeInBytes = UINT(sizeof(Mesh::VertexData) * modelData.meshes[0].GetVerticies().size());
    skinnedBufferView_.StrideInBytes = sizeof(Mesh::VertexData);

    skinningInformation_.Create(L"skinningInformation", sizeof(SkinningInformation));
    skinningInformation_.Copy(uint32_t(modelData.meshes[0].GetVerticies().size()));
}

void SkinCluster::Update()
{
    WellForGPU* mappedPalette = static_cast<WellForGPU*>(paletteResource_.GetCPUData());

    for (size_t jointIndex = 0; jointIndex < skeleton_->GetAllJoint().size(); ++jointIndex) {
        assert(jointIndex < inverseBindPoseMatrices_.size());
        mappedPalette[jointIndex].skeletonSpaceMatrix = inverseBindPoseMatrices_[jointIndex] * skeleton_->GetAllJoint()[jointIndex].skeletonSpaceMatrix;
        mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = Transpose(Inverse(mappedPalette[jointIndex].skeletonSpaceMatrix));
        mappedPalette[jointIndex].skeletonSpaceMatrix = mappedPalette[jointIndex].skeletonSpaceMatrix;
    }
}
