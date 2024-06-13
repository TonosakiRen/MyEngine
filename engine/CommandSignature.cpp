#include "CommandSignature.h"

#include "Helper.h"
#include <assert.h>

#include "DirectXCommon.h"

void CommandSignature::Create(const D3D12_COMMAND_SIGNATURE_DESC& desc) {

    Microsoft::WRL::ComPtr<ID3DBlob> blob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;


  /*  Helper::AssertIfFailed(DirectXCommon::GetInstance()->GetDevice()->CreateCommandSignature(
        0,
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        IID_PPV_ARGS(commandSignature_.ReleaseAndGetAddressOf())));*/
}
