#include "Raytracing/Raytracing.h"
#include "Texture/TextureManager.h"
#include "Model/ModelManager.h"
#include "Light/ShaderManager.h"
#include "Light/LightManager.h"

using namespace Microsoft::WRL;

void Raytracing::Initialize() {
    tras_ = std::make_unique<TRAS>();
    tras_->Create(L"tras");

    instanceDescs_ = std::make_unique<RaytracingInstanceDescs>();
    instanceDescs_->Create();
}

void Raytracing::Update(CommandContext& commandContext)
{
    instanceDescs_->Update();
    tras_->Build(commandContext, *instanceDescs_.get());
}

void Raytracing::AddInstanceDesc(const BLAS& blas, const WorldTransform& worldTranform)
{
    D3D12_RAYTRACING_INSTANCE_DESC desc;
    desc.AccelerationStructure = blas.GetGPUVirtualAddress();
    desc.InstanceContributionToHitGroupIndex = 0;
    desc.InstanceMask = 0xFF;
    Matrix4x4 mat = Transpose(worldTranform.matWorld_);
    memcpy(desc.Transform, &mat, sizeof(desc.Transform));
    instanceDescs_->AddDesc(desc);
}

void Raytracing::Finalize()
{
   rootSignature_.reset();
   pipelineState_.reset();   
   instanceDescs_.reset();
   tras_.reset();
}

void Raytracing::CreatePipeline() {
    HRESULT result = S_FALSE;
    ComPtr<IDxcBlob> msBlob;
    ComPtr<IDxcBlob> psBlob;
    ComPtr<IDxcBlob> asBlob;

    auto shaderManager = ShaderManager::GetInstance();

    asBlob = shaderManager->Compile(L"MeshTestAS.hlsl", ShaderManager::kAmplification);
    assert(asBlob != nullptr);

    msBlob = shaderManager->Compile(L"MeshTestMS.hlsl", ShaderManager::kMesh);
    assert(msBlob != nullptr);

    psBlob = shaderManager->Compile(L"MeshTestPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    rootSignature_ = std::make_unique<RootSignature>();
    pipelineState_ = std::make_unique<PipelineState>();
}
