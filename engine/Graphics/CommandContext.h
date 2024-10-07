#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl/client.h>

#include <cassert>
#include <vector>

#include "GPUResource.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "ColorBuffer.h"
#include "CubeColorBuffer.h"
#include "DepthBuffer.h"

#include "DWParam.h"

class CommandContext
{
public:
    void Create();
    void SetDescriptorHeap();

    void Start();

    void ShutDown();

    void Close();
    void Reset();

    void TransitionResource(GPUResource& resource, D3D12_RESOURCE_STATES newState);
    void FlushResourceBarriers();

    void CopyBuffer(GPUResource& dest, GPUResource& src);
    void CopyBufferRegion(GPUResource& dest, size_t destOffset, GPUResource& src, size_t srcOffset, size_t numBytes);

    void SetPipelineState(const PipelineState& pipelineState);
    void SetGraphicsRootSignature(const RootSignature& rootSignature);
    void SetComputeRootSignature(const RootSignature& rootSignature);

    void ClearColor(ColorBuffer& target);
    void ClearColor(ColorBuffer& target, float clearColor[4]);
    void ClearColor(CubeColorBuffer& target);
    void ClearColor(CubeColorBuffer& target, float clearColor[4]);
    void CopyCubeBuffer(CubeColorBuffer& dest, CubeColorBuffer& src, uint32_t srcIndex = 0);
    void CopyCubeBuffer(CubeColorBuffer& dest, ColorBuffer& src);
    void ClearDepth(DepthBuffer& target);
    void ClearDepth(DepthBuffer& target, float clearValue);

    void SetRenderTargets(UINT numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[]);
    void SetRenderTargets(UINT numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[], D3D12_CPU_DESCRIPTOR_HANDLE dsv);
    void SetRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE rtv) { SetRenderTargets(1, &rtv); }
    void SetRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv) { SetRenderTargets(1, &rtv, dsv); }

    void SetDepthStencil(D3D12_CPU_DESCRIPTOR_HANDLE dsv);

    void SetViewport(const D3D12_VIEWPORT& viewport);
    void SetViewport(FLOAT x, FLOAT y, FLOAT w, FLOAT h, FLOAT minDepth = 0.0f, FLOAT maxDepth = 1.0f);
    void SetScissorRect(const D3D12_RECT& rect);
    void SetScissorRect(UINT left, UINT top, UINT right, UINT bottom);
    void SetViewportAndScissorRect(const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect);
    void SetViewportAndScissorRect(UINT x, UINT y, UINT w, UINT h);

    void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);

    void SetConstantArray(UINT rootIndex, UINT numConstants, const void* constants);
    void SetConstant(UINT rootIndex, UINT offset, DWParam value);
    void SetConstants(UINT rootIndex, DWParam x);
    void SetConstants(UINT rootIndex, DWParam x, DWParam y);
    void SetConstants(UINT rootIndex, DWParam x, DWParam y, DWParam z);
    void SetConstants(UINT rootIndex, DWParam x, DWParam y, DWParam z, DWParam w);

    void SetComputeConstantArray(UINT rootIndex, UINT numConstants, const void* constants);
    void SetComputeConstant(UINT rootIndex, UINT offset, DWParam value);
    void SetComputeConstants(UINT rootIndex, DWParam x);
    void SetComputeConstants(UINT rootIndex, DWParam x, DWParam y);
    void SetComputeConstants(UINT rootIndex, DWParam x, DWParam y, DWParam z);
    void SetComputeConstants(UINT rootIndex, DWParam x, DWParam y, DWParam z, DWParam w);

    void SetConstantBuffer(UINT rootIndex, D3D12_GPU_VIRTUAL_ADDRESS address);
    void SetComputeConstantBuffer(UINT rootIndex, D3D12_GPU_VIRTUAL_ADDRESS address);
    void SetComputeUAVBuffer(UINT rootIndex, D3D12_GPU_VIRTUAL_ADDRESS address);
    void SetDescriptorTable(UINT rootIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor);
    void SetComputeDescriptorTable(UINT rootIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor);

    void SetVertexBuffer(UINT slot, const D3D12_VERTEX_BUFFER_VIEW& vbv);
    void SetVertexBuffer(UINT slot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW vbvs[]);
    void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& ibv);

    void UAVBarrier(GPUResource& resource);
    void UAVBarrier(ID3D12Resource& resource);

    void Dispatch(uint32_t x,uint32_t y,uint32_t z);
    void DispatchMesh(uint32_t x, uint32_t y, uint32_t z);

    void Draw(UINT vertexCount, UINT vertexStartOffset = 0);
    void DrawIndexed(UINT indexCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0);
    void DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation = 0, UINT startInstanceLocation = 0);
    void DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0, UINT startInstanceLocation = 0);

    void ExecuteIndirect(ID3D12CommandSignature* commandSignature, UINT maxCommandCount, ID3D12Resource* argumentBuffer, UINT64 argumentBufferOffset, ID3D12Resource* countBuffer, UINT64 countBufferOffset);

    void BuildRaytracingAccelerationStructure(const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& asDesc);

    operator ID3D12GraphicsCommandList6* () const { return currentCommandList_; }
    
private:
    static const uint32_t kMaxNumResourceBarriers = 16;

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_[2];
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> commandList_[2];
    ID3D12CommandAllocator* currentCommandAllocator_ = nullptr;
    ID3D12GraphicsCommandList6* currentCommandList_ = nullptr;
    ID3D12CommandAllocator* executeCommandAllocator_ = nullptr;
    ID3D12GraphicsCommandList6* executeCommandList_ = nullptr;

    D3D12_RESOURCE_BARRIER resourceBarriers_[kMaxNumResourceBarriers]{};
    uint32_t numResourceBarriers_;

    ID3D12RootSignature* rootSignature_;
    ID3D12PipelineState* pipelineState_;

};

inline void CommandContext::TransitionResource(GPUResource& resource, D3D12_RESOURCE_STATES newState) {
    auto oldState = resource.state_;

    if (newState != oldState) {
        assert(numResourceBarriers_ < kMaxNumResourceBarriers);
        auto& barrier = resourceBarriers_[numResourceBarriers_++];
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = resource;
        barrier.Transition.StateBefore = oldState;
        barrier.Transition.StateAfter = newState;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        resource.state_ = newState;
    }

    if (numResourceBarriers_ >= kMaxNumResourceBarriers) {
        FlushResourceBarriers();
    }
}

inline void CommandContext::FlushResourceBarriers() {
    if (numResourceBarriers_ > 0) {
        currentCommandList_->ResourceBarrier(numResourceBarriers_, resourceBarriers_);
        numResourceBarriers_ = 0;
    }
}

inline void CommandContext::CopyBuffer(GPUResource& dest, GPUResource& src) {
    TransitionResource(dest, D3D12_RESOURCE_STATE_COPY_DEST);
    TransitionResource(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
    FlushResourceBarriers();
    currentCommandList_->CopyResource(dest, src);
}

inline void CommandContext::CopyBufferRegion(GPUResource& dest, size_t destOffset, GPUResource& src, size_t srcOffset, size_t numBytes) {
    TransitionResource(dest, D3D12_RESOURCE_STATE_COPY_DEST);
    TransitionResource(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
    FlushResourceBarriers();
    currentCommandList_->CopyBufferRegion(dest, destOffset, src, srcOffset, numBytes);
}

inline void CommandContext::SetPipelineState(const PipelineState& pipelineState) {
    ID3D12PipelineState* ps = pipelineState;
    if (pipelineState_ != ps) {
        pipelineState_ = ps;
        currentCommandList_->SetPipelineState(pipelineState_);
    }
}

inline void CommandContext::SetGraphicsRootSignature(const RootSignature& rootSignature) {
    ID3D12RootSignature* rs = rootSignature;
    if (rootSignature_ != rs) {
        rootSignature_ = rs;
        currentCommandList_->SetGraphicsRootSignature(rootSignature_);
    }
}

inline void CommandContext::SetComputeRootSignature(const RootSignature& rootSignature) {
    ID3D12RootSignature* rs = rootSignature;
    if (rootSignature_ != rs) {
        rootSignature_ = rs;
        currentCommandList_->SetComputeRootSignature(rootSignature_);
    }
}

inline void CommandContext::ClearColor(ColorBuffer& target) {
    FlushResourceBarriers();
    currentCommandList_->ClearRenderTargetView(target.GetRTV(), target.GetClearColor(), 0, nullptr);
}

inline void CommandContext::ClearColor(ColorBuffer& target, float clearColor[4]) {
    FlushResourceBarriers();
    currentCommandList_->ClearRenderTargetView(target.GetRTV(), clearColor, 0, nullptr);
}

inline void CommandContext::ClearColor(CubeColorBuffer& target) {
    FlushResourceBarriers();
    for (uint32_t i = 0; i < 6; i++) {
        currentCommandList_->ClearRenderTargetView(target.GetRTV(i), target.GetClearColor(), 0, nullptr);
    }
}

inline void CommandContext::ClearColor(CubeColorBuffer& target, float clearColor[4]) {
    FlushResourceBarriers();
    for (uint32_t i = 0; i < 6; i++) {
        currentCommandList_->ClearRenderTargetView(target.GetRTV(i), clearColor, 0, nullptr);
    }
}

inline void CommandContext::CopyCubeBuffer(CubeColorBuffer& dest, CubeColorBuffer& src,uint32_t srcIndex) {
    TransitionResource(dest, D3D12_RESOURCE_STATE_COPY_DEST);
    TransitionResource(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
    FlushResourceBarriers();
    for (uint32_t i = 1; i < 6; i++) {
        D3D12_TEXTURE_COPY_LOCATION dstLocation{};
        dstLocation.pResource = dest;
        dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dstLocation.SubresourceIndex = D3D12CalcSubresource(0, i, 0, 1, 1);

        D3D12_TEXTURE_COPY_LOCATION srcLocation{};
        srcLocation.pResource = src;
        srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        srcLocation.SubresourceIndex = D3D12CalcSubresource(0, srcIndex, 0, 1, 1);

        currentCommandList_->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
    }
}

inline void CommandContext::CopyCubeBuffer(CubeColorBuffer& dest, ColorBuffer& src) {
    TransitionResource(dest, D3D12_RESOURCE_STATE_COPY_DEST);
    TransitionResource(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
    FlushResourceBarriers();

    D3D12_TEXTURE_COPY_LOCATION srcLocation{};
    srcLocation.pResource = src;
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    srcLocation.SubresourceIndex = 0;

    for (uint32_t i = 0; i < 6; i++) {
        D3D12_TEXTURE_COPY_LOCATION dstLocation{};
        dstLocation.pResource = dest;
        dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dstLocation.SubresourceIndex = D3D12CalcSubresource(0, i, 0, 1, 1);

        currentCommandList_->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
    }
}

inline void CommandContext::ClearDepth(DepthBuffer& target) {
    FlushResourceBarriers();
    currentCommandList_->ClearDepthStencilView(target.GetDSV(), D3D12_CLEAR_FLAG_DEPTH, target.GetClearValue(), 0, 0, nullptr);
}

inline void CommandContext::ClearDepth(DepthBuffer& target, float clearValue) {
    FlushResourceBarriers();
    currentCommandList_->ClearDepthStencilView(target.GetDSV(), D3D12_CLEAR_FLAG_DEPTH, clearValue, 0, 0, nullptr);
}

inline void CommandContext::SetRenderTargets(UINT numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[]) {
    currentCommandList_->OMSetRenderTargets(numRTVs, rtvs, FALSE, nullptr);
}

inline void CommandContext::SetRenderTargets(UINT numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[], D3D12_CPU_DESCRIPTOR_HANDLE dsv) {
    currentCommandList_->OMSetRenderTargets(numRTVs, rtvs, FALSE, &dsv);
}


inline void CommandContext::SetDepthStencil(D3D12_CPU_DESCRIPTOR_HANDLE dsv)
{
    currentCommandList_->OMSetRenderTargets(0, nullptr, FALSE, &dsv);
}

inline void CommandContext::SetViewport(const D3D12_VIEWPORT& viewport) {
    currentCommandList_->RSSetViewports(1, &viewport);
}

inline void CommandContext::SetViewport(FLOAT x, FLOAT y, FLOAT w, FLOAT h, FLOAT minDepth, FLOAT maxDepth) {
    D3D12_VIEWPORT viewport{};
    viewport.TopLeftX = x;
    viewport.TopLeftY = y;
    viewport.Width = w;
    viewport.Height = h;
    viewport.MinDepth = minDepth;
    viewport.MaxDepth = maxDepth;
    SetViewport(viewport);
}

inline void CommandContext::SetScissorRect(const D3D12_RECT& rect) {
    assert(rect.left < rect.right && rect.top < rect.bottom);
    currentCommandList_->RSSetScissorRects(1, &rect);
}

inline void CommandContext::SetScissorRect(UINT left, UINT top, UINT right, UINT bottom) {
    D3D12_RECT rect{};
    rect.left = LONG(left);
    rect.top = LONG(top);
    rect.right = LONG(right);
    rect.bottom = LONG(bottom);
    SetScissorRect(rect);
}

inline void CommandContext::SetViewportAndScissorRect(const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) {
    SetViewport(viewport);
    SetScissorRect(rect);
}

inline void CommandContext::SetViewportAndScissorRect(UINT x, UINT y, UINT w, UINT h) {
    SetViewport(float(x), float(y), float(w), float(h));
    SetScissorRect(x, y, x + w, y + h);
}

inline void CommandContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology) {
    currentCommandList_->IASetPrimitiveTopology(topology);
}

inline void CommandContext::SetConstantArray(UINT rootIndex, UINT numConstants, const void* constants) {
    currentCommandList_->SetGraphicsRoot32BitConstants(rootIndex, numConstants, constants, 0);
}

inline void CommandContext::SetConstant(UINT rootIndex, UINT offset, DWParam value) {
    currentCommandList_->SetGraphicsRoot32BitConstant(rootIndex, value.v.u, offset);
}

inline void CommandContext::SetConstants(UINT rootIndex, DWParam x) {
    currentCommandList_->SetGraphicsRoot32BitConstant(rootIndex, x.v.u, 0);
}

inline void CommandContext::SetConstants(UINT rootIndex, DWParam x, DWParam y) {
    currentCommandList_->SetGraphicsRoot32BitConstant(rootIndex, x.v.u, 0);
    currentCommandList_->SetGraphicsRoot32BitConstant(rootIndex, y.v.u, 1);
}

inline void CommandContext::SetConstants(UINT rootIndex, DWParam x, DWParam y, DWParam z) {
    currentCommandList_->SetGraphicsRoot32BitConstant(rootIndex, x.v.u, 0);
    currentCommandList_->SetGraphicsRoot32BitConstant(rootIndex, y.v.u, 1);
    currentCommandList_->SetGraphicsRoot32BitConstant(rootIndex, z.v.u, 2);
}

inline void CommandContext::SetConstants(UINT rootIndex, DWParam x, DWParam y, DWParam z, DWParam w) {
    currentCommandList_->SetGraphicsRoot32BitConstant(rootIndex, x.v.u, 0);
    currentCommandList_->SetGraphicsRoot32BitConstant(rootIndex, y.v.u, 1);
    currentCommandList_->SetGraphicsRoot32BitConstant(rootIndex, z.v.u, 2);
    currentCommandList_->SetGraphicsRoot32BitConstant(rootIndex, w.v.u, 3);
}

inline void CommandContext::SetComputeConstantArray(UINT rootIndex, UINT numConstants, const void* constants) {
    currentCommandList_->SetComputeRoot32BitConstants(rootIndex, numConstants, constants, 0);
}

inline void CommandContext::SetComputeConstant(UINT rootIndex, UINT offset, DWParam value) {
    currentCommandList_->SetComputeRoot32BitConstant(rootIndex, value.v.u, offset);
}

inline void CommandContext::SetComputeConstants(UINT rootIndex, DWParam x) {
    currentCommandList_->SetComputeRoot32BitConstant(rootIndex, x.v.u, 0);
}

inline void CommandContext::SetComputeConstants(UINT rootIndex, DWParam x, DWParam y) {
    currentCommandList_->SetComputeRoot32BitConstant(rootIndex, x.v.u, 0);
    currentCommandList_->SetComputeRoot32BitConstant(rootIndex, y.v.u, 1);
}

inline void CommandContext::SetComputeConstants(UINT rootIndex, DWParam x, DWParam y, DWParam z) {
    currentCommandList_->SetComputeRoot32BitConstant(rootIndex, x.v.u, 0);
    currentCommandList_->SetComputeRoot32BitConstant(rootIndex, y.v.u, 1);
    currentCommandList_->SetComputeRoot32BitConstant(rootIndex, z.v.u, 2);
}

inline void CommandContext::SetComputeConstants(UINT rootIndex, DWParam x, DWParam y, DWParam z, DWParam w) {
    currentCommandList_->SetComputeRoot32BitConstant(rootIndex, x.v.u, 0);
    currentCommandList_->SetComputeRoot32BitConstant(rootIndex, y.v.u, 1);
    currentCommandList_->SetComputeRoot32BitConstant(rootIndex, z.v.u, 2);
    currentCommandList_->SetComputeRoot32BitConstant(rootIndex, w.v.u, 3);
}

inline void CommandContext::SetConstantBuffer(UINT rootIndex, D3D12_GPU_VIRTUAL_ADDRESS address) {
    currentCommandList_->SetGraphicsRootConstantBufferView(rootIndex, address);
}

inline void CommandContext::SetComputeConstantBuffer(UINT rootIndex, D3D12_GPU_VIRTUAL_ADDRESS address) {
    currentCommandList_->SetComputeRootConstantBufferView(rootIndex, address);
}

inline void CommandContext::SetComputeUAVBuffer(UINT rootIndex, D3D12_GPU_VIRTUAL_ADDRESS address)
{
    currentCommandList_->SetComputeRootUnorderedAccessView(rootIndex, address);
}

inline void CommandContext::SetDescriptorTable(UINT rootIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor) {
    currentCommandList_->SetGraphicsRootDescriptorTable(rootIndex, baseDescriptor);
}

inline void CommandContext::SetComputeDescriptorTable(UINT rootIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor) {
    currentCommandList_->SetComputeRootDescriptorTable(rootIndex, baseDescriptor);
}

inline void CommandContext::SetVertexBuffer(UINT slot, const D3D12_VERTEX_BUFFER_VIEW& vbv) {
    currentCommandList_->IASetVertexBuffers(slot, 1, &vbv);
}

inline void CommandContext::SetVertexBuffer(UINT slot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW vbvs[]) {
    currentCommandList_->IASetVertexBuffers(slot, numViews, vbvs);
}

inline void CommandContext::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& ibv) {
    currentCommandList_->IASetIndexBuffer(&ibv);
}

inline void CommandContext::Dispatch(uint32_t x, uint32_t y, uint32_t z)
{
    FlushResourceBarriers();
    currentCommandList_->Dispatch(x,y,z);
}

inline void CommandContext::DispatchMesh(uint32_t x, uint32_t y, uint32_t z)
{
    FlushResourceBarriers();
    currentCommandList_->DispatchMesh(x, y, z);
}

inline void CommandContext::UAVBarrier(GPUResource& resource) {
    D3D12_RESOURCE_BARRIER& barrierDesc = resourceBarriers_[numResourceBarriers_++];
    barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barrierDesc.UAV.pResource = resource;
    if (numResourceBarriers_ >= kMaxNumResourceBarriers) {
        FlushResourceBarriers();
    }
}

inline void CommandContext::UAVBarrier(ID3D12Resource& resource) {
    D3D12_RESOURCE_BARRIER& barrierDesc = resourceBarriers_[numResourceBarriers_++];
    barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barrierDesc.UAV.pResource = &resource;
    if (numResourceBarriers_ >= kMaxNumResourceBarriers) {
        FlushResourceBarriers();
    }
}


inline void CommandContext::Draw(UINT vertexCount, UINT vertexStartOffset) {
    DrawInstanced(vertexCount, 1, vertexStartOffset, 0);
}

inline void CommandContext::DrawIndexed(UINT indexCount, UINT startIndexLocation, INT baseVertexLocation) {
    DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
}

inline void CommandContext::DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation) {
    FlushResourceBarriers();
    currentCommandList_->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

inline void CommandContext::DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation) {
    FlushResourceBarriers();
    currentCommandList_->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

inline void CommandContext::ExecuteIndirect(ID3D12CommandSignature* commandSignature, UINT maxCommandCount, ID3D12Resource* argumentBuffer, UINT64 argumentBufferOffset, ID3D12Resource* countBuffer, UINT64 countBufferOffset) {
    FlushResourceBarriers();
    currentCommandList_->ExecuteIndirect(commandSignature, maxCommandCount, argumentBuffer, argumentBufferOffset, countBuffer, countBufferOffset);
}


inline void CommandContext::BuildRaytracingAccelerationStructure(const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& asDesc) {
    FlushResourceBarriers();
    currentCommandList_->BuildRaytracingAccelerationStructure(&asDesc, 0, nullptr);
}