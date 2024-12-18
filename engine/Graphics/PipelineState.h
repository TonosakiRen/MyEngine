#pragma once
/**
 * @file PipelineState.h
 * @brief Pipeline構造体
 */
#include <d3d12.h>
#include <wrl/client.h>

#include <string>
class PipelineState
{
public:
	void Create(const std::wstring& name, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
	void Create(const std::wstring& name, const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc);
	void Create(const std::wstring& name, const D3D12_PIPELINE_STATE_STREAM_DESC& desc);

	operator ID3D12PipelineState* () const { return pipelineState_.Get(); }
	operator bool() const { return pipelineState_; }
private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
	std::wstring name_;
};

enum PipelineType {
	kForward,
	kDeferred,
	//透明描画のパイプライン
	kOpacityPipelineNum,

	kDepth = 2,

	//不透明描画のパイプライン
	kPipelineNum
};

