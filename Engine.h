#pragma once
#include <cstdint>
#include <string>
#include <cassert>
#include <memory>
#include <chrono>
#include <thread>

#include "WinApp.h"
#include "RenderManager.h"
#include "TextureManager.h"
#include "Model.h"
#include "GameObject.h"
namespace Engine {
	WinApp* winApp = nullptr;
	RenderManager* renderManager = nullptr;
	TextureManager* textureManager = nullptr;
	

	static uint32_t kWindowWidth = 1280;
	static uint32_t kWindowHeight = 720;
	void Initialize();
	void Shutdown();
	bool BeginFrame();
	void BeginDraw();
	void EndDraw();
	void RequestQuit();
	uint32_t LoadTexture(const std::string& name);
	bool isEndRequest = false;
	std::chrono::steady_clock::time_point referenceTime;
}