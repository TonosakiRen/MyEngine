#pragma once
class BaseScene;
class SceneManager
{
public:

	static SceneManager* GetInstance();

	void Finalize();

	void Update();

	void ModelDraw();
	void SkyDraw();
	void ShadowDraw();
	void SpotLightShadowDraw();

	void ParticleDraw();
	void ParticleBoxDraw();

	void PreSpriteDraw();
	void PostSpriteDraw();

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

