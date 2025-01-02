#pragma once
/**
 * @file Helper.h
 * @brief 便利関数
 */
#include <d3dx12.h>

#include <winnt.h>

#include <string>
#include <format>

namespace Helper {

    extern const D3D12_RASTERIZER_DESC RasterizerDefault;
    extern const D3D12_RASTERIZER_DESC RasterizerNoCull;
    extern const D3D12_RASTERIZER_DESC RasterizerWireframe;

    extern const D3D12_BLEND_DESC BlendNoColorWrite;
    extern const D3D12_BLEND_DESC BlendDisable;     // ブレンド無効
    extern const D3D12_BLEND_DESC BlendAlpha;       // アルファブレンド
    extern const D3D12_BLEND_DESC BlendMultiply;    // 乗算合成
    extern const D3D12_BLEND_DESC BlendAdditive;    // 加算合成
    extern const D3D12_BLEND_DESC BlendSubtract;    // 加算合成

    extern const D3D12_DEPTH_STENCIL_DESC DepthStateDisabled;
    extern const D3D12_DEPTH_STENCIL_DESC DepthStateReadWrite;

    //アライメント
    template<typename T>
    T AlignUp(T value, size_t alignment) {
        size_t mask = alignment - 1;
        return T((size_t(value) + mask) & ~mask);
    }
    //アライメントされているか
    template<typename T>
    bool IsAligned(T value, size_t alignment) {
        return (size_t(value) & (alignment - 1)) == 0;
    }

    //各適性のFormatを出力
    DXGI_FORMAT GetBaseFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetUAVFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetDepthFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetStencilFormat(DXGI_FORMAT format);
    //1pixel何バイトか
    size_t GetBytePerPixel(DXGI_FORMAT format);

    //HRESULTがfalseならassertで止まる
    void AssertIfFailed(HRESULT hr);

    //Logを出す
    void Log(const std::string& message);
    void Log(const std::wstring& message);
    //string型変換
    std::wstring ConvertString(const std::string& str);
    std::string ConvertString(const std::wstring& str);
}