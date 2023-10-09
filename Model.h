#pragma once

#include <vector>
#include <memory>
#include <filesystem>

#include "UploadBuffer.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "Texture.h"
#include "MyMath.h"
#include "Camera.h"

class CommandContext;

class Model {
public:
    enum class SamplerType {
        kLinearWrap,
        kLinearClamp,
        kPointWrap,
        kPointClamp
    };
    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector2 texcoord;
    };

    static void CreatePipeline(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);
    static void DestroyPipeline();

    void CreateFromObj(const std::filesystem::path& path);
    void Draw(CommandContext& commandContext, const Matrix4x4& world, const Camera& camera, bool isLighting = true);

    void SetSamplerType(SamplerType samplerType) { samplerType_ = samplerType; }

private:
    struct Mesh {
        uint32_t vertexOffset;
        uint32_t indexOffset;
        uint32_t indexCount;
        uint32_t materialIndex;
    };
    struct Material {
        std::string name;
        Vector4 color;
        std::unique_ptr<Texture> texture;
    };

    static std::unique_ptr<RootSignature> rootSignature_;
    static std::unique_ptr<PipelineState> pipelineState_;

    void LoadMTLFile(const std::filesystem::path& path);

    UploadBuffer vertexBuffer_;
    D3D12_VERTEX_BUFFER_VIEW vbView_{};
    UploadBuffer indexBuffer_;
    D3D12_INDEX_BUFFER_VIEW ibView_{};
    std::vector<Mesh> meshes_;
    std::vector<Material> materials_;
    SamplerType samplerType_ = SamplerType::kLinearWrap;
};