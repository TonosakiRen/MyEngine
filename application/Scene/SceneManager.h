#pragma once
/**
 * @file SceneManager.h
 * @brief Sceneの管理
 */
class BaseScene;
class SceneManager
{
public:

	static SceneManager* GetInstance();

	void Finalize();

	void Update();
	void Draw();

	//Setter
	void SetNextScene(BaseScene* nextScene) { nextScene_ = nextScene; };
	//Getter
	void* GetNextScene() const{
		return nextScene_;
	}

private:
	SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

	BaseScene* scene_ = nullptr;
	BaseScene* nextScene_ = nullptr;
};

