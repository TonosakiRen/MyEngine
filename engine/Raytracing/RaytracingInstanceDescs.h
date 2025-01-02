#pragma once
/**
 * @file RaytracingInstanceDescs.h
 * @brief RaytracingInstanceDescsの構造体
 */
#include "GPUResource/GPUResource.h"
#include "GPUResource/UploadBuffer.h"
#include "Graphics/CommandContext.h"
#include "Mymath.h"

class RaytracingInstanceDescs {
public:

    static const uint32_t kMaxDescNum_ = 8192;

    void Create();
    void Update();
    //InstanceDescを追加
    void AddDesc(D3D12_RAYTRACING_INSTANCE_DESC& desc);
    //Getter
    uint32_t GetInstanceNum() const { return instanceNum_; }
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const{
        return descsBuffer_.GetGPUVirtualAddress();
    }
protected:
    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> descs_;
    uint32_t instanceNum_ = 0;
    UploadBuffer descsBuffer_;
};