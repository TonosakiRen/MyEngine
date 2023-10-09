#include "CommandContext.h"

#include "Helper.h"
#include "Graphics.h"
#include "DescriptorHeap.h"

void CommandContext::Create() {
    auto device = Graphics::GetInstance()->GetDevice();
    ASSERT_IF_FAILED(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_.ReleaseAndGetAddressOf())));

    ASSERT_IF_FAILED(device->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(),
        nullptr, IID_PPV_ARGS(commandList_.ReleaseAndGetAddressOf())));
}

void CommandContext::Close() {
    FlushResourceBarriers();
    ASSERT_IF_FAILED(commandList_->Close());
}

void CommandContext::Reset() {
    ASSERT_IF_FAILED(commandAllocator_->Reset());
    ASSERT_IF_FAILED(commandList_->Reset(commandAllocator_.Get(), nullptr));

    dynamicBuffer_.Reset();

    auto graphics = Graphics::GetInstance();

    ID3D12DescriptorHeap* ppHeaps[] = {
        (ID3D12DescriptorHeap*)graphics->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
        (ID3D12DescriptorHeap*)graphics->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) };

    commandList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    rootSignature_ = nullptr;
    pipelineState_ = nullptr;
}


