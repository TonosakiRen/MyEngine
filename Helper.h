#pragma once
#include "Externals/DirectXTex/d3dx12.h"
#include <winnt.h>
#include <string>
#include <format>
namespace Helper {

    void Log(const std::string& message);
    void Log(const std::wstring& message);
    std::wstring ConvertString(const std::string& str);
    std::string ConvertString(const std::wstring& str);

    //基本的な使い方
    //wstring->string
    //Log(ConvertString(std::format(L"WSTRING{}\n",wstringValue)))

    extern const D3D12_RASTERIZER_DESC RasterizerDefault;
    extern const D3D12_RASTERIZER_DESC RasterizerNoCull;
    extern const D3D12_RASTERIZER_DESC RasterizerWireframe;

    extern const D3D12_BLEND_DESC BlendNoColorWrite;
    extern const D3D12_BLEND_DESC BlendDisable;    
    extern const D3D12_BLEND_DESC BlendAlpha;       
    extern const D3D12_BLEND_DESC BlendMultiply;   
    extern const D3D12_BLEND_DESC BlendAdditive;    
    extern const D3D12_BLEND_DESC BlendSubtract;   

    extern const D3D12_DEPTH_STENCIL_DESC DepthStateDisabled;
    extern const D3D12_DEPTH_STENCIL_DESC DepthStateReadWrite;

    DXGI_FORMAT GetBaseFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetUAVFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetDepthFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetStencilFormat(DXGI_FORMAT format);
    size_t GetBytePerPixel(DXGI_FORMAT format);

	void AssertIfFailed(HRESULT hr, const wchar_t* str);

    DXGI_FORMAT GetBaseFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetUAVFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetDepthFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetStencilFormat(DXGI_FORMAT format);
    size_t GetBytePerPixel(DXGI_FORMAT format);

    template <typename T>
    inline T AlignUpWithMask(T value, size_t mask)
    {
        return (T)(((size_t)value + mask) & ~mask);
    }

    template <typename T>
    inline T AlignDownWithMask(T value, size_t mask)
    {
        return (T)((size_t)value & ~mask);
    }

    template <typename T>
    inline T AlignUp(T value, size_t alignment)
    {
        return AlignUpWithMask(value, alignment - 1);
    }

    template <typename T>
    inline T AlignDown(T value, size_t alignment)
    {
        return AlignDownWithMask(value, alignment - 1);
    }

    template <typename T>
    inline bool IsAligned(T value, size_t alignment)
    {
        return 0 == ((size_t)value & (alignment - 1));
    }

    template <typename T>
    inline T DivideByMultiple(T value, size_t alignment)
    {
        return (T)((value + alignment - 1) / alignment);
    }
}


#ifndef HELPER_DEFINES
#define HELPER_DEFINES

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define WIDE_(x) L##x
#define WIDE(x) WIDE_(x)

#define ASSERT_IF_FAILED(hr) Helper::AssertIfFailed(hr, WIDE(__FILE__) "(" STRINGIFY(__LINE__) ")\n" STRINGIFY(hr))

#ifdef _DEBUG
#define D3D12_OBJECT_SET_NAME(object, name) object->SetName(name)
#else
#define D3D12_OBJECT_SET_NAME(object, name) ((void)object),((void)name)
#endif // _DEBUG


#endif // HELPER_DEFINES