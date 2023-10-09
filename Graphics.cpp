#include "Graphics.h"
#include <dxgi1_6.h>
#include "Helper.h"
#include <cassert>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

using namespace Microsoft::WRL;
#ifdef _DEBUG

#include <dxgidebug.h>

namespace {
    struct LeakChecker {
        ~LeakChecker() {
            ComPtr<IDXGIDebug1> debug;
            if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {
                debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
                debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
                debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
            }
        }
    } leakChecker;

}
#endif // _DEBUG

Graphics* Graphics::GetInstance() {
    static Graphics instance;
    return &instance;
}

void Graphics::Initialize() {
    CreateDevice();
}

void Graphics::Shutdown() {
    commandQueue_.Signal();
    commandQueue_.WaitForGPU();
}

void Graphics::CreateDevice() {
#ifdef _DEBUG
    ComPtr<ID3D12Debug1> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())))) {
        debugController->EnableDebugLayer();
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
#endif // _DEBUG
    ComPtr<IDXGIFactory7> factory;
    ASSERT_IF_FAILED(CreateDXGIFactory(IID_PPV_ARGS(factory.GetAddressOf())));

    //使用するアダプタ用の変数、最初にnullptrを入れておく
    ComPtr<IDXGIAdapter4> adapter = nullptr;
    //良い順にアダプタを頼む
    for (UINT i = 0; factory->EnumAdapterByGpuPreference(i,
        DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) !=
        DXGI_ERROR_NOT_FOUND; ++i) {
        //アダプターの情報を取得する	
        DXGI_ADAPTER_DESC3 adapterDesc{};
        ASSERT_IF_FAILED(adapter->GetDesc3(&adapterDesc));
        //ソフトウェアアダプタでなければ採用！
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            Helper::Log(std::format(L"Use Adapter{}\n", adapterDesc.Description));
            break;
        }
        adapter.Reset(); // ソフトウェアアダプタは見なかったことにする
    }
    //適切なアダプタが見つからんかったので起動できない
    assert(adapter);

    // 機能レベルとログ出力用の文字列
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
    };
    const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
    // 高い順に生成できるか試していく
    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        // 採用したアダプターデバイスを生成
        // 指定した機能レベルでデバイスが生成できたかを確認
        if (SUCCEEDED(D3D12CreateDevice(
            adapter.Get(), featureLevels[i], IID_PPV_ARGS(device_.GetAddressOf())))) {
            // 生成できたのでログ出力を行ってループを抜ける
            Helper::Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
            break;
        }
    }
    //デバイスの生成がうまくいかなかったので起動できない
    assert(device_);
#ifdef _DEBUG
    // デバッグ時のみ
    ComPtr<ID3D12InfoQueue> infoQueue;
    if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf())))) {
        // やばいエラーの時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        // エラーの時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // 警告時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        // 抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };
        // 抑制するレベル
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        // 指定したメッセージの表示を抑制する
        infoQueue->PushStorageFilter(&filter);
    }
#endif
}