#include "Graphics/CommandSignature.h"

#include "Graphics/Helper.h"
#include <assert.h>

#include "Graphics/DirectXCommon.h"

void CommandSignature::Create(const std::wstring& name, const D3D12_COMMAND_SIGNATURE_DESC& desc,ID3D12RootSignature& rootSignature) {

    Helper::AssertIfFailed(DirectXCommon::GetInstance()->GetDevice()->CreateCommandSignature(
        &desc,
        &rootSignature,
        IID_PPV_ARGS(&commandSignature_)
       ));
#ifdef _DEBUG
    commandSignature_->SetName(name.c_str());
    name_ = name;
#endif // _DEBUG
}
