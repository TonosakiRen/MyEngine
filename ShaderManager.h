#pragma once

#include <d3d12.h>
#include <dxcapi.h>
#include <wrl/client.h>

#include <string>
#include <vector>

class ShaderManager {
public:
    static ShaderManager* GetInstance();

    enum Type {
        kVertex,
        kPixel,
        kGeometry,
        kHull,
        kDomain,
        kCompute,

        kNumTypes
    };

    void Initialize();
    Microsoft::WRL::ComPtr<IDxcBlob> Compile(const std::wstring& path, Type type);

private:
    Microsoft::WRL::ComPtr<IDxcBlob> Compile(const std::wstring& path, const std::wstring& profile);

    Microsoft::WRL::ComPtr<IDxcUtils> utils_;
    Microsoft::WRL::ComPtr<IDxcCompiler3> compiler_;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;
    std::vector<std::wstring> loadedShaderList_;
private:
    // シングルトン
    ShaderManager() = default;
    ~ShaderManager() = default;
    ShaderManager(const ShaderManager&) = delete;
    const ShaderManager& operator=(const ShaderManager&) = delete;
};