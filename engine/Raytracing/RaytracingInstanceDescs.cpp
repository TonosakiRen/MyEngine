#include "Raytracing/RaytracingInstanceDescs.h"

#include <cassert>
#include "Graphics/DirectXCommon.h"
#include "GPUResource/BufferManager.h"

void RaytracingInstanceDescs::Create()
{
	descsBuffer_.Create(L"RaytracingInstanceDesc", sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * kMaxDescNum_);
}

void RaytracingInstanceDescs::AddDesc(D3D12_RAYTRACING_INSTANCE_DESC& descs)
{
	descs.InstanceID = descs_.size();
	descs_.push_back(descs);
}

void RaytracingInstanceDescs::Update()
{
	D3D12_RAYTRACING_INSTANCE_DESC* descs = static_cast<D3D12_RAYTRACING_INSTANCE_DESC*>(descsBuffer_.GetCPUData());
	memcpy(descs, descs_.data(), sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * descs_.size());
	instanceNum_ = uint32_t(descs_.size());
}
