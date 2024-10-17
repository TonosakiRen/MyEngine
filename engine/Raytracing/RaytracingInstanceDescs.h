#pragma once
#include "GPUResource/GPUResource.h"
#include "GPUResource/UploadBuffer.h"
#include "Graphics/CommandContext.h"
#include "Mymath.h"

class RaytracingInstanceDescs {
public:

    static const uint32_t kMaxDescNum_ = 128;

    void Create();
    void AddDesc(const D3D12_RAYTRACING_INSTANCE_DESC& desc);
    void Update();

    uint32_t GetInstanceNum() const { return instanceNum_; }
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const{
        return descsBuffer_.GetGPUVirtualAddress();
    }
protected:
    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> descs_;
    uint32_t instanceNum_ = 0;
    UploadBuffer descsBuffer_;
};