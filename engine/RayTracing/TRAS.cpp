#include "TRAS.h"

#include <cassert>
#include "DirectXCommon.h"
#include "BufferManager.h"


void TRAS::Create(const std::wstring& name, const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc,CommandContext& commandContext) {
    HRESULT result = S_FALSE;

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{};
    inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
    inputs.NumDescs = 1;
    inputs.pGeometryDescs = &geomDesc;
    inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info{};
    DirectXCommon::GetInstance()->GetDevice()->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

    scrach_.CreateResource(L"BlasSclachBuffer", CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), CD3DX12_RESOURCE_DESC::Buffer(sizeof(info), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    auto desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(info.ResultDataMaxSizeInBytes),D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    resource_ = BufferManager::GetInstance()->CreateResource(
        index_,
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc{};
    asDesc.Inputs = inputs;
    asDesc.ScratchAccelerationStructureData = scrach_->GetGPUVirtualAddress();
    asDesc.DestAccelerationStructureData = resource_->GetGPUVirtualAddress();

    commandContext.BuildRaytracingAccelerationStructure(asDesc);
    commandContext.UAVBarrier(*resource_.Get());

#ifdef _DEBUG
    resource_->SetName(name.c_str());
    name_ = name;
#endif // _DEBUG

    state_ = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
    bufferSize_ = info.ResultDataMaxSizeInBytes;
}
   
