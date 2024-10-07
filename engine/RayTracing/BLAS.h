#pragma once
#include "GPUResource.h"
#include "CopyBuffer.h"
#include "CommandContext.h"

class BLAS : public GPUResource {
public:

    void Create(const std::wstring& name, const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc, CommandContext& commandContext);
    size_t GetBufferSize() const { return bufferSize_; }

protected:
    GPUResource scrach_;
    size_t bufferSize_ = 0;
};