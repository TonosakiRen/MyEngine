#include "SwapChain.h"
#include <cassert>
#include "Graphics.h"
#include "Helper.h"
#include "ColorBuffer.h"

using namespace Microsoft::WRL;
void SwapChain::Create(HWND hwnd) {
	ComPtr<IDXGIFactory7> factory;
	ASSERT_IF_FAILED(CreateDXGIFactory(IID_PPV_ARGS(factory.GetAddressOf())));

	RECT clientRect{};
	assert(GetClientRect(hwnd, &clientRect));
		
	LONG clientWidth = clientRect.right - clientRect.left;
	LONG clientHeight = clientRect.bottom - clientRect.top;

	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.Width = UINT(clientWidth);
	desc.Height = UINT(clientHeight);
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = kNumBuffers;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	ASSERT_IF_FAILED(factory->CreateSwapChainForHwnd(
		Graphics::GetInstance()->GetCommandQueue(),
		hwnd,
		&desc,
		nullptr,
		nullptr,
		reinterpret_cast<IDXGISwapChain1**>(swapChain_.ReleaseAndGetAddressOf())));

	swapChain_->SetMaximumFrameLatency(1);

	for (uint32_t i = 0; i < kNumBuffers; ++i) {
		ComPtr<ID3D12Resource> resource;
		ASSERT_IF_FAILED(swapChain_->GetBuffer(i, IID_PPV_ARGS(resource.GetAddressOf())));
		buffers_[i] = std::make_unique<ColorBuffer>();
		buffers_[i]->CreateFromSwapChain(L"SwapChainBuffer" + std::to_wstring(i), resource.Detach());
	}
}

void SwapChain::Present() {
	static constexpr int32_t kThreasholdRefreshRate = 58;
	swapChain_->Present(refreshRate_ < kThreasholdRefreshRate ? 0 : 1, 0);
	currentBufferIndex_ = (currentBufferIndex_ + 1) % kNumBuffers;
}