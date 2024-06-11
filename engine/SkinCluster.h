#pragma once
#include <vector>
#include <span>
#include <array>

#include "Mymath.h"
#include "GPUResource.h"
#include "DescriptorHandle.h"
#include "StructuredBuffer.h"
#include "Skeleton.h"

const uint32_t kNumMaxInfluence = 4;
struct VertexInfluence {
    std::array<float, kNumMaxInfluence> weights;
    std::array<int32_t, kNumMaxInfluence> jointIndices;
};
struct WellForGPU {
    Matrix4x4 skeletonSpaceMatrix;
    Matrix4x4 skeletonSpaceInverseTransposeMatrix;
};

class SkinCluster {
public:
    
    void Create(const Skeleton& skeleton,const uint32_t modelHandle);
    void Update();
     
    const DescriptorHandle& GetSRV() const { return paletteResource_.GetSRV(); }

public:

    std::vector<Matrix4x4> inverseBindPoseMatrices_;
    UploadBuffer influenceResource_;
    D3D12_VERTEX_BUFFER_VIEW influenceBufferView_;
    StructuredBuffer paletteResource_;
    const Skeleton* skeleton_ = nullptr;
};
