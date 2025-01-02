#pragma once
/**
 * @file BaseScene.h
 * @brief sceneの仮想Class
 */
class SceneManager;
class BaseScene
{
public:

	virtual ~BaseScene() = default;

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	//setter
	virtual void SetSceneManager(SceneManager* sceneManager) {
		sceneManager_ = sceneManager;
	}


protected:
	SceneManager* sceneManager_ = nullptr;
};

