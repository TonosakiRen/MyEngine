#pragma once
#include "GPUResource/GPUResource.h"
#include "GPUResource/UploadBuffer.h"
#include "Graphics/CommandContext.h"
#include "Mymath.h"
#include "Raytracing/RaytracingInstanceDescs.h"

class TLAS : public UploadBuffer {
public:

    void Create(const std::wstring& name);
    void Build(CommandContext& commandContext, const RaytracingInstanceDescs& descs);
    size_t GetBufferSize() const { return bufferSize_; }

protected:
    GPUResource scrach_;
    size_t bufferSize_ = 0;
};