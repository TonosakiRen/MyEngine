#pragma once
class SceneManager;
class BaseScene
{
public:
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update() = 0;

	virtual void ModelDraw() = 0;

	virtual void ShadowDraw() = 0;
	virtual void SpotLightShadowDraw() = 0;

	virtual void ParticleDraw() = 0;
	virtual void ParticleBoxDraw() = 0;

	virtual void PreSpriteDraw() = 0;
	virtual void PostSpriteDraw() = 0;

	virtual void SetSceneManager(SceneManager* sceneManager) {
		sceneManager_ = sceneManager;
	}

	virtual ~BaseScene() = default;

protected:
	SceneManager* sceneManager_ = nullptr;
};

