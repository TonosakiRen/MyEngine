#pragma once

#include "Graphics/WinApp.h"
#include <imgui.h>
class CommandContext;
class ImGuiManager
{
public:
	static ImGuiManager* GetInstance();
	void Initialize(WinApp* winApp);
	void Finalize();
	void Begin();
	void Draw(CommandContext& commandContext);

private:
	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	const ImGuiManager& operator=(const ImGuiManager&) = delete;
};

