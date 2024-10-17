#pragma once
#include "PostEffect/GaussianBlur.h"
#include "GPUResource/UploadBuffer.h"

class CommandContext;

class Bloom
{
public:

    struct VertexData {
        Vector4 pos;
        Vector2 uv;
    };

    static const uint32_t kMaxLevel = 4;

    void Initialize(ColorBuffer* originalTexture);
    void Render(CommandContext& commandContext, ColorBuffer* originalTexture, uint32_t level = kMaxLevel);

    void SetThreshold(float threshold) { threshold_ = threshold; }
    void SetKnee(float knee) { knee_ = knee; }


    ColorBuffer& GetLuminanceTexture() { return luminanceTexture_; }

private:
    RootSignature rootSignature_;
    PipelineState luminacePipelineState_;
    PipelineState additivePipelineState_;

    ColorBuffer luminanceTexture_;
    GaussianBlur gaussianBlurs_[kMaxLevel];

    float threshold_ = 0.8f;
    float knee_ = 0.3f;
    float intensity_ = 1.0f;
};

