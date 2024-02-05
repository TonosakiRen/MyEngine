#pragma once
#include <memory>
class WinApp;
class Input;
class Audio;
class SceneManager;
class Framework
{
public:

	void Run();

	virtual void Initialize();
	virtual void Finalize();
	virtual void Update();
	virtual void Draw() = 0;

	virtual bool IsEndRequest() { return endRequest_; }
	virtual ~Framework() = default;

	static bool endRequest_;
public:

	WinApp* win = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;
};

