#include "CommandQueue.h"

#include <cassert>

#include "Helper.h"
#include "Graphics.h"
#include "CommandContext.h"

CommandQueue::~CommandQueue() {
    Destroy();
}

void CommandQueue::Create() {
    auto device = Graphics::GetInstance()->GetDevice();

    D3D12_COMMAND_QUEUE_DESC desc{};
    ASSERT_IF_FAILED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(commandQueue_.ReleaseAndGetAddressOf())));

    ASSERT_IF_FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf())));


    if (fenceEvent_) {
        fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        assert(fenceEvent_);
    }

    fenceValue_ = 0;
}

void CommandQueue::Excute(const CommandContext& commandContext) {
    ID3D12CommandList* ppCmdList[] = { commandContext };
    commandQueue_->ExecuteCommandLists(_countof(ppCmdList), ppCmdList);
}

void CommandQueue::Signal() {
    ASSERT_IF_FAILED(commandQueue_->Signal(fence_.Get(), ++fenceValue_));
}

void CommandQueue::WaitForGPU() {
    if (fence_->GetCompletedValue() < fenceValue_) {
        ASSERT_IF_FAILED(fence_->SetEventOnCompletion(fenceValue_, fenceEvent_));
        WaitForSingleObject(fenceEvent_, INFINITE);
    }
}

void CommandQueue::Destroy() {
    WaitForGPU();
    if (fenceEvent_) {
        CloseHandle(fenceEvent_);
        fenceEvent_ = nullptr;
    }
}