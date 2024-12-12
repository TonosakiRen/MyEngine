#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <string>


class CommandSignature {
public:
    void Create(const std::wstring& name, const D3D12_COMMAND_SIGNATURE_DESC& desc,ID3D12RootSignature& rootSignature);

    operator ID3D12CommandSignature* () const { return commandSignature_.Get(); }
    operator bool() const { return commandSignature_; }

private:
    Microsoft::WRL::ComPtr<ID3D12CommandSignature> commandSignature_;
    std::wstring name_;
};