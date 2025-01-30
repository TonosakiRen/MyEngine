#pragma once
/**
 * @file Framework.h
 * @brief Frameworkを行う
 */
#include <memory>
#include "audio/Audio.h"
#include "Input/Input.h"

class WinApp;
class SceneManager;
class Framework
{
public:

	static uint32_t kFrame;
	static uint32_t kFrameRemainder;

	//Frameworkを走らせる
	void Run();

	virtual void Initialize();
	virtual void Finalize();
	virtual void Update();
	virtual void Draw() = 0;

	//Getter
	virtual bool IsEndRequest() { return endRequest_; }
	virtual ~Framework() = default;

	static bool endRequest_;
public:

	WinApp* win = nullptr;
	Engine::Input* input = nullptr;
	Engine::Audio* audio = nullptr;
};

