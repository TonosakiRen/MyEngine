#pragma once
class BaseScene;
class SceneManager
{
public:

	static SceneManager* GetInstance();

	void Finalize();

	void Update();

	void Draw();

	void SetNextScene(BaseScene* nextScene) { nextScene_ = nextScene; };

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

