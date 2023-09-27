#include "ImGuiManager.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize(WinApp* winApp) {

	shaderResourceManager_ = ShaderResourceManager::GetInstance();
	dxCommon_ = DirectXCommon::GetInstance();

	// ImGuiのコンテキストを生成
	ImGui::CreateContext();
	// ImGuiのスタイルを設定
	ImGui::StyleColorsDark();
	// プラットフォームとレンダラーのバックエンドを設定する
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(
		dxCommon_->GetDevice(), static_cast<int>(dxCommon_->GetFrameBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, shaderResourceManager_->GetDescriptorHeap().Get(),
		shaderResourceManager_->GetCPUDescriptorHandle(ShaderResourceManager::ImGuiSrv),
		shaderResourceManager_->GetGPUDescriptorHandle(ShaderResourceManager::ImGuiSrv));

	ImGuiIO& io = ImGui::GetIO();
	// 標準フォントを追加する
	io.Fonts->AddFontDefault();
}

void ImGuiManager::Finalize() {
	// 後始末
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}

void ImGuiManager::Begin() {
	// ImGuiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End() {

	// 描画前準備
	ImGui::Render();
}

void ImGuiManager::Draw() {

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { shaderResourceManager_->GetDescriptorHeap().Get()};
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	// 描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}
