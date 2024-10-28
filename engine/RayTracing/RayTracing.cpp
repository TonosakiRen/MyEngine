#include "Raytracing/Raytracing.h"
#include "Texture/TextureManager.h"
#include "Model/ModelManager.h"
#include "Light/ShaderManager.h"
#include "Light/LightManager.h"

using namespace Microsoft::WRL;

static const wchar_t kRayGenerationName[] = L"RayGeneration";
static const wchar_t kMissName[] = L"Miss";
static const wchar_t kClosestHitName[] = L"ClosestHit";
static const wchar_t kHitGroup[] = L"HitGroup";

void Raytracing::Initialize() {
    tras_ = std::make_unique<TLAS>();
    tras_->Create(L"tlas");

    instanceDescs_ = std::make_unique<RaytracingInstanceDescs>();
    instanceDescs_->Create();
}

void Raytracing::Update(CommandContext& commandContext)
{
    instanceDescs_->Update();
    tras_->Build(commandContext, *instanceDescs_.get());
}

void Raytracing::AddInstanceDesc(const BLAS& blas, const WorldTransform& worldTransform)
{
    D3D12_RAYTRACING_INSTANCE_DESC desc;
    desc.AccelerationStructure = blas.GetGPUVirtualAddress();
    desc.InstanceContributionToHitGroupIndex = 0;
    desc.InstanceMask = 0xFF;
    Matrix4x4 mat = Transpose(worldTransform.matWorld_);
    memcpy(desc.Transform, &mat, sizeof(desc.Transform));
    instanceDescs_->AddDesc(desc);
}

void Raytracing::AddInstanceDesc(const BLAS& blas, const Matrix4x4& matrix)
{
    D3D12_RAYTRACING_INSTANCE_DESC desc;
    desc.AccelerationStructure = blas.GetGPUVirtualAddress();
    desc.InstanceContributionToHitGroupIndex = 0;
    desc.InstanceMask = 0xFF;
    Matrix4x4 mat = Transpose(matrix);
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
    ComPtr<IDxcBlob> raytracingBlob;

    auto shaderManager = ShaderManager::GetInstance();

    raytracingBlob = shaderManager->Compile(L"Raytracing/Raytracing.hlsl", ShaderManager::kAmplification);
    assert(raytracingBlob != nullptr);


    rootSignature_ = std::make_unique<RootSignature>();
    pipelineState_ = std::make_unique<PipelineState>();

    // デスクリプタレンジ
    CD3DX12_DESCRIPTOR_RANGE descRangeSRV[int(RootParameter::kDescriptorRangeNum)];
    descRangeSRV[int(RootParameter::kResult)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); 
    descRangeSRV[int(RootParameter::kTras)].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

    // ルートパラメータ
    CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::parameterNum)] = {};
    rootparams[int(RootParameter::kResult)].InitAsDescriptorTable(1, &descRangeSRV[int(RootParameter::kResult)], D3D12_SHADER_VISIBILITY_ALL);
    rootparams[int(RootParameter::kTras)].InitAsDescriptorTable(1, &descRangeSRV[int(RootParameter::kTras)], D3D12_SHADER_VISIBILITY_ALL);

    // スタティックサンプラー
    CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
        CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT);

    // ルートシグネチャの設定
    D3D12_ROOT_SIGNATURE_DESC grobalRootSignatureDesc;
    grobalRootSignatureDesc.pParameters = rootparams;
    grobalRootSignatureDesc.NumParameters = _countof(rootparams);
    grobalRootSignatureDesc.pStaticSamplers = &samplerDesc;
    grobalRootSignatureDesc.NumStaticSamplers = 1;
    grobalRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

    rootSignature_->Create(L"raytracingGlobalRootSignature", grobalRootSignatureDesc);

    CD3DX12_STATE_OBJECT_DESC stateObjectDesc{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

    auto dxiLibrarySubobject = stateObjectDesc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();

    D3D12_SHADER_BYTECODE libdxil{};
    libdxil.BytecodeLength = raytracingBlob->GetBufferSize();
    libdxil.pShaderBytecode = raytracingBlob->GetBufferPointer();
    dxiLibrarySubobject->SetDXILLibrary(&libdxil);
    dxiLibrarySubobject->DefineExport(kRayGenerationName);
    dxiLibrarySubobject->DefineExport(kMissName);
    dxiLibrarySubobject->DefineExport(kClosestHitName);

    auto hitGroupSubobject = stateObjectDesc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();

    hitGroupSubobject->SetClosestHitShaderImport(kClosestHitName);
    hitGroupSubobject->SetHitGroupExport(kHitGroup);
    hitGroupSubobject->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

    auto shaderConfigSubobject = stateObjectDesc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
    uint32_t payloadSize = static_cast<uint32_t>(sizeof(Vector3));
    uint32_t attributeSize = static_cast<uint32_t>(sizeof(Vector3));
    shaderConfigSubobject->Config(payloadSize, attributeSize);

    auto localRootSignature = stateObjectDesc.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
    //localRootSignature->SetRootSignature(rayGenLo)

}
