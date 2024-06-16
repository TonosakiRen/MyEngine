#include "CommandSignature.h"

#include "Helper.h"
#include <assert.h>

#include "DirectXCommon.h"

void CommandSignature::Create(const D3D12_COMMAND_SIGNATURE_DESC& desc,ID3D12RootSignature& rootSignature) {

    Helper::AssertIfFailed(DirectXCommon::GetInstance()->GetDevice()->CreateCommandSignature(
        &desc,
        &rootSignature,
        IID_PPV_ARGS(&commandSignature_)
       ));
}
