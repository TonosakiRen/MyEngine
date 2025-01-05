#pragma once
/**
 * @file SkinCluster.h
 * @brief Skinningをするためのモジュール
 */
#include <vector>
#include <span>
#include <array>

#include "Mymath.h"
#include "GPUResource/GPUResource.h"
#include "Graphics/DescriptorHandle.h"
#include "GPUResource/StructuredBuffer.h"
#include "Animation/Skeleton.h"
#include "GPUResource/RwStructuredBuffer.h"

const uint32_t kNumMaxInfluence = 4;
struct VertexInfluence {
    std::array<float, kNumMaxInfluence> weights;
    std::array<int32_t, kNumMaxInfluence> jointIndices;
};
struct WellForGPU {
    Matrix4x4 skeletonSpaceMatrix;
    Matrix4x4 skeletonSpaceInverseTransposeMatrix;
};

struct SkinningInformation {
    uint32_t numVertices;
};

class SkinCluster {
public:
    
    void Create(Skeleton& skeleton,const uint32_t modelHandle);
    void Update();
    
    //Getter
    RwStructuredBuffer& GetSkinnedVertices(){ return skinnedVertices_; }
    const DescriptorHandle& GetPaletteResourceSRV() const { return paletteResource_.GetSRV(); }
    const DescriptorHandle& GetInfluencesSRV() const { return influences_.GetSRV(); }
    const DescriptorHandle& GetSkinnedVerticesUAV() const { return skinnedVertices_.GetUAV(); }
    const D3D12_VERTEX_BUFFER_VIEW& GetSkinnedVertexBufferView() const { return skinnedBufferView_; }
    const D3D12_GPU_VIRTUAL_ADDRESS GetSkinningInformationGPUAddress() const { return skinningInformation_.GetGPUVirtualAddress(); }

private:

    std::vector<Matrix4x4> inverseBindPoseMatrices_;
    UploadBuffer influenceResource_;
    D3D12_VERTEX_BUFFER_VIEW influenceBufferView_;
    StructuredBuffer paletteResource_;

    
    RwStructuredBuffer skinnedVertices_;
    D3D12_VERTEX_BUFFER_VIEW skinnedBufferView_;;

    StructuredBuffer influences_;
    UploadBuffer skinningInformation_;
    

    Skeleton* skeleton_ = nullptr;
};
