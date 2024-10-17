#include "Graphics/CommandContext.h"

#include "Graphics/DirectXCommon.h"
#include "Graphics/DescriptorHeap.h"
#include "Framework/Framework.h"

#include "Graphics/Helper.h"

void CommandContext::Create() {

    auto device = DirectXCommon::GetInstance()->GetDevice();
    Helper::AssertIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_[0].ReleaseAndGetAddressOf())));

    Helper::AssertIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_[1].ReleaseAndGetAddressOf())));

    Helper::AssertIfFailed(device->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_[0].Get(),
        nullptr, IID_PPV_ARGS(commandList_[0].ReleaseAndGetAddressOf())));

    Helper::AssertIfFailed(device->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_[1].Get(),
        nullptr, IID_PPV_ARGS(commandList_[1].ReleaseAndGetAddressOf())));

    currentCommandList_ = commandList_[0].Get();
    currentCommandAllocator_ = commandAllocator_[0].Get();
}

void CommandContext::SetDescriptorHeap()
{
    auto graphics = DirectXCommon::GetInstance();
    ID3D12DescriptorHeap* ppHeaps[] = {
        (ID3D12DescriptorHeap*)graphics->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
     };
    commandList_[0]->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    commandList_[1]->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void CommandContext::Start()
{
    executeCommandList_ = currentCommandList_;
    executeCommandAllocator_ = currentCommandAllocator_;
    currentCommandList_= commandList_[Framework::kFrameRemainder].Get();
    currentCommandAllocator_ = commandAllocator_[Framework::kFrameRemainder].Get();
    rootSignature_ = nullptr;
    pipelineState_ = nullptr;
}

void CommandContext::ShutDown()
{
    commandAllocator_[0].Reset();
    commandAllocator_[1].Reset();
    commandList_[0].Reset();
    commandList_[1].Reset();
}

void CommandContext::Close() {
    FlushResourceBarriers();
    Helper::AssertIfFailed(currentCommandList_->Close());
}

void CommandContext::Reset() {
    
    Helper::AssertIfFailed(executeCommandAllocator_->Reset());

    Helper::AssertIfFailed(executeCommandList_->Reset(executeCommandAllocator_, nullptr));

    auto graphics = DirectXCommon::GetInstance();
    ID3D12DescriptorHeap* ppHeaps[] = {
        (ID3D12DescriptorHeap*)graphics->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
     };
    executeCommandList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}
