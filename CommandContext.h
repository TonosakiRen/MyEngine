#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cassert>
#include <vector>

#include "GPUResource.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "DepthBuffer.h"
#include "LinearAllocator.h"
#include "Helper.h"
#include "ColorBuffer.h"

class CommandContext
{
public:
    void Create();

    void Close();
    void Reset();

    void TransitionResource(GPUResource& resource, D3D12_RESOURCE_STATES newState);
 
    void FlushResourceBarriers();
    void CopyBuffer(GPUResource& dest, GPUResource& src);
    void CopyBufferRegion(GPUResource& dest, size_t destOffset, GPUResource& src, size_t srcOffset, size_t numBytes);
    void SetPipelineState(const PipelineState& pipelineState);
    void SetRootSignature(const RootSignature& rootSignature);

    void ClearColor(ColorBuffer& target);
    void ClearColor(ColorBuffer& target, float clearColor[4]);
    void ClearDepth(DepthBuffer& target);
    void ClearDepth(DepthBuffer& target, float clearValue);
 
    void SetRenderTargets(UINT numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[]);
    void SetRenderTargets(UINT numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[], D3D12_CPU_DESCRIPTOR_HANDLE dsv);
    void SetRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE rtv) { SetRenderTargets(1, &rtv); }
    void SetRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv) { SetRenderTargets(1, &rtv, dsv); }

    void SetViewport(const D3D12_VIEWPORT& viewport);
    void SetViewport(FLOAT x, FLOAT y, FLOAT w, FLOAT h, FLOAT minDepth = 0.0f, FLOAT maxDepth = 1.0f);
    void SetScissorRect(const D3D12_RECT& rect);
    void SetScissorRect(UINT left, UINT top, UINT right, UINT bottom);
    void SetViewportAndScissorRect(const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect);
    void SetViewportAndScissorRect(UINT x, UINT y, UINT w, UINT h);

    void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);
    void SetConstantBuffer(UINT rootIndex, D3D12_GPU_VIRTUAL_ADDRESS address);
    void SetDescriptorTable(UINT rootIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor);

    void SetVertexBuffer(UINT slot, const D3D12_VERTEX_BUFFER_VIEW& vbv);
    void SetVertexBuffer(UINT slot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW vbvs[]);
    void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& ibv);

    void SetDynamicConstantBufferView(UINT rootIndex,size_t bufferSize,const void* bufferData);
    void SetDynamicShaderResourceView(UINT rootIndex, size_t bufferSize, const void* bufferData);
    void SetDynamicVertexBuffer(UINT slot, size_t numVertices, size_t vertexStride, const void* vertexData);
    void SetDynamicIndexBuffer(size_t numIndices, DXGI_FORMAT indexFormat, const void* indexData);

    void Draw(UINT vertexCount, UINT vertexStartOffset = 0);
    void DrawIndexed(UINT indexCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0);
    void DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation);
    void DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation);
    operator ID3D12GraphicsCommandList* () const { return commandList_.Get(); }
private:
    static const uint32_t kMaxNumResourceBarriers = 16;

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

    D3D12_RESOURCE_BARRIER resourceBarriers_[kMaxNumResourceBarriers]{};
    uint32_t numResourceBarriers_;

    ID3D12RootSignature* rootSignature_;
    ID3D12PipelineState* pipelineState_;

    LinearAllocator dynamicBuffer_;
};

inline void CommandContext::TransitionResource(GPUResource& resource, D3D12_RESOURCE_STATES newState) {
    auto oldState = resource.state_;
    if (newState != oldState) {
        assert(numResourceBarriers_ < kMaxNumResourceBarriers);
        auto& barrier = resourceBarriers_[numResourceBarriers_++];
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = resource;
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
        commandList_->ResourceBarrier(numResourceBarriers_, resourceBarriers_);
        numResourceBarriers_ = 0;
    }
}

inline void CommandContext::CopyBuffer(GPUResource& dest, GPUResource& src) {
    TransitionResource(dest, D3D12_RESOURCE_STATE_COPY_DEST);
    TransitionResource(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
    FlushResourceBarriers();
    commandList_->CopyResource(dest, src);
}

inline void CommandContext::CopyBufferRegion(GPUResource& dest, size_t destOffset, GPUResource& src, size_t srcOffset, size_t numBytes) {
    TransitionResource(dest, D3D12_RESOURCE_STATE_COPY_DEST);
    TransitionResource(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
    FlushResourceBarriers();
    commandList_->CopyBufferRegion(dest, destOffset, src, srcOffset, numBytes);
}

inline void CommandContext::SetPipelineState(const PipelineState& pipelineState) {
    ID3D12PipelineState* ps = pipelineState;
    if (pipelineState_ != ps) {
        pipelineState_ = ps;
        commandList_->SetPipelineState(pipelineState_);
    }
}

inline void CommandContext::SetRootSignature(const RootSignature& rootSignature) {
    ID3D12RootSignature* rs = rootSignature;
    if (rootSignature_ != rs) {
        rootSignature_ = rs;
        commandList_->SetGraphicsRootSignature(rootSignature_);
    }
}

inline void CommandContext::ClearColor(ColorBuffer& target) {
    FlushResourceBarriers();
    commandList_->ClearRenderTargetView(target.GetRTV(), target.GetClearColor(), 0, nullptr);
}

inline void CommandContext::ClearColor(ColorBuffer& target, float clearColor[4]) {
    FlushResourceBarriers();
    commandList_->ClearRenderTargetView(target.GetRTV(), clearColor, 0, nullptr);
}

inline void CommandContext::ClearDepth(DepthBuffer& target) {
    FlushResourceBarriers();
    commandList_->ClearDepthStencilView(target.GetDSV(), D3D12_CLEAR_FLAG_DEPTH, target.GetClearValue(), 0, 0, nullptr);
}

inline void CommandContext::ClearDepth(DepthBuffer& target, float clearValue) {
    FlushResourceBarriers();
    commandList_->ClearDepthStencilView(target.GetDSV(), D3D12_CLEAR_FLAG_DEPTH, clearValue, 0, 0, nullptr);
}

inline void CommandContext::SetRenderTargets(UINT numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[]) {
    commandList_->OMSetRenderTargets(numRTVs, rtvs, FALSE, nullptr);
}

inline void CommandContext::SetRenderTargets(UINT numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE rtvs[], D3D12_CPU_DESCRIPTOR_HANDLE dsv) {
    commandList_->OMSetRenderTargets(numRTVs, rtvs, FALSE, &dsv);
}

inline void CommandContext::SetViewport(const D3D12_VIEWPORT& viewport) {
    commandList_->RSSetViewports(1, &viewport);
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
    commandList_->RSSetScissorRects(1, &rect);
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

inline void CommandContext::SetConstantBuffer(UINT rootIndex, D3D12_GPU_VIRTUAL_ADDRESS address) {
    commandList_->SetGraphicsRootConstantBufferView(rootIndex, address);
}

inline void CommandContext::SetDescriptorTable(UINT rootIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor) {
    commandList_->SetGraphicsRootDescriptorTable(rootIndex, baseDescriptor);
}

inline void CommandContext::SetVertexBuffer(UINT slot, const D3D12_VERTEX_BUFFER_VIEW& vbv) {
    commandList_->IASetVertexBuffers(slot, 1, &vbv);
}

inline void CommandContext::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& ibv) {
    commandList_->IASetIndexBuffer(&ibv);
}

inline void CommandContext::SetDynamicConstantBufferView(UINT rootIndex, size_t bufferSize, const void* bufferData) {
    assert(bufferData);

    auto allocation = dynamicBuffer_.Allocate(bufferSize, 256);
    memcpy(allocation.cpu, bufferData, bufferSize);
    commandList_->SetGraphicsRootConstantBufferView(rootIndex, allocation.gpu);
}

inline void CommandContext::SetDynamicVertexBuffer(UINT slot, size_t numVertices, size_t vertexStride, const void* vertexData) {
    assert(vertexData);

    size_t bufferSize = Helper::AlignUp(numVertices * vertexStride, 16);
    auto allocation = dynamicBuffer_.Allocate(bufferSize);
    memcpy(allocation.cpu, vertexData, bufferSize);
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{
        .BufferLocation = allocation.gpu,
        .SizeInBytes = UINT(bufferSize),
        .StrideInBytes = UINT(vertexStride)
    };
    commandList_->IASetVertexBuffers(slot, 1, &vertexBufferView);
}

inline void CommandContext::SetDynamicIndexBuffer(size_t numIndices, DXGI_FORMAT indexFormat, const void* indexData) {
    assert(indexData);
    assert(indexFormat == DXGI_FORMAT_R16_UINT || indexFormat == DXGI_FORMAT_R32_UINT);

    size_t stride = (indexFormat == DXGI_FORMAT_R16_UINT) ? sizeof(uint16_t) : sizeof(uint32_t);
    size_t bufferSize = Helper::AlignUp(numIndices * stride, 16);
    auto allocation = dynamicBuffer_.Allocate(bufferSize);
    memcpy(allocation.cpu, indexData, bufferSize);
    D3D12_INDEX_BUFFER_VIEW indexBufferView;
    indexBufferView.BufferLocation = allocation.gpu;
    indexBufferView.SizeInBytes = UINT(numIndices * stride);
    indexBufferView.Format = indexFormat;
    commandList_->IASetIndexBuffer(&indexBufferView);
}

inline void CommandContext::Draw(UINT vertexCount, UINT vertexStartOffset) {
    FlushResourceBarriers();
    commandList_->DrawInstanced(vertexCount, 1, vertexStartOffset, 0);
}

inline void CommandContext::DrawIndexed(UINT indexCount, UINT startIndexLocation, INT baseVertexLocation) {
    FlushResourceBarriers();
    commandList_->DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
}

inline void CommandContext::DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation) {
    FlushResourceBarriers();
    commandList_->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

inline void CommandContext::DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation) {
    FlushResourceBarriers();
    commandList_->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}
