#pragma once
class SceneManager;
class BaseScene
{
public:
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update() = 0;

	virtual void Draw() = 0;

	virtual void SetSceneManager(SceneManager* sceneManager) {
		sceneManager_ = sceneManager;
	}

	virtual ~BaseScene() = default;

protected:
	SceneManager* sceneManager_ = nullptr;
};

