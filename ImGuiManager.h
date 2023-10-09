#pragma once

#include "Externals/ImGui/imgui.h"

#include <Windows.h>
#include <d3d12.h>

class CommandContext;

class ImGuiManager {
public:
    static ImGuiManager* GetInstance();

    void Initialize(HWND hWnd, DXGI_FORMAT rtvFormat);
    void NewFrame();
    void Render(CommandContext& commandContext);
    void Shutdown();

private:
    // シングルトン
    ImGuiManager() = default;
    ~ImGuiManager() = default;
    ImGuiManager(const ImGuiManager&) = delete;
    const ImGuiManager& operator=(const ImGuiManager&) = delete;
};