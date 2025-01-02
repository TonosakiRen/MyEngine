#pragma once
/**
 * @file BLAS.h
 * @brief BLAS構造体
 */
#include "GPUResource/GPUResource.h"
#include "GPUResource/CopyBuffer.h"
#include "Graphics/CommandContext.h"

class BLAS : public GPUResource {
public:

    void Create(const std::wstring& name, const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc, CommandContext& commandContext);
    //Getter
    size_t GetBufferSize() const { return bufferSize_; }

protected:
    GPUResource scrach_;
    size_t bufferSize_ = 0;
};