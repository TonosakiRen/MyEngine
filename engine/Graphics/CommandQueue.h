#pragma once
/**
 * @file CommandQueue.h
 * @brief CommandQueueをラップ
 */
#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <chrono>

class CommandQueue {
public:
    ~CommandQueue();

    void Create();

    //ComandList実行
    void Excute(ID3D12GraphicsCommandList* commandList);
    //GPUからのSignal受信
    void Signal();
    //GPUの処理を待つ
    void WaitForGPU();
    //FPS固定
    void UpdateFixFPS();

    operator ID3D12CommandQueue* () const { return commandQueue_.Get(); }

private:
    //object解放
    void Destroy();

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
    std::chrono::steady_clock::time_point reference_;
    HANDLE fenceEvent_;
    uint64_t fenceValue_;
};
