#include "PipelineState.h"

#include "DirectXCommon.h"
#include <assert.h>
void PipelineState::Create(const std::wstring& name, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) {
    HRESULT hr = S_FALSE;
    hr = (DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf())));
    assert(SUCCEEDED(hr));
#ifdef _DEBUG
    pipelineState_->SetName(name.c_str());
    name_ = name;
#endif // _DEBUG
}

void PipelineState::Create(const std::wstring& name, const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc)
{
    HRESULT hr = S_FALSE;
    hr = (DirectXCommon::GetInstance()->GetDevice()->CreateComputePipelineState(&desc, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf())));
    assert(SUCCEEDED(hr));
#ifdef _DEBUG
    pipelineState_->SetName(name.c_str());
    name_ = name;
#endif // _DEBUG
}

void PipelineState::Create(const std::wstring& name, const D3D12_PIPELINE_STATE_STREAM_DESC& desc)
{
    HRESULT hr = S_FALSE;
    hr = (DirectXCommon::GetInstance()->GetDevice()->CreatePipelineState(&desc, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf())));
    assert(SUCCEEDED(hr));
#ifdef _DEBUG
    pipelineState_->SetName(name.c_str());
    name_ = name;
#endif // _DEBUG
}
