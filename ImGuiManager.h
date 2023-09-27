#pragma once

#include "DirectXCommon.h"
#include "WinApp.h"
#include "externals/imgui/imgui.h"
#include "ShaderResourceManager.h"
class ImGuiManager
{
public:
	static ImGuiManager* GetInstance();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// ImGui受付開始
	/// </summary>
	void Begin();

	/// <summary>
	/// ImGui受付終了
	/// </summary>
	void End();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	// DirectX基盤インスタンス（借りてくる）
	DirectXCommon* dxCommon_ = nullptr;
	ShaderResourceManager* shaderResourceManager_ = nullptr;

private:
	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	const ImGuiManager& operator=(const ImGuiManager&) = delete;
};

