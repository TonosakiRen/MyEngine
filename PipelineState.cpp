#include "PipelineState.h"
#include "Graphics.h"
#include "Helper.h"

void PipelineState::Create(const std::wstring& name, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) {
    ASSERT_IF_FAILED(Graphics::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf())));
    D3D12_OBJECT_SET_NAME(pipelineState_, name.c_str());
}
