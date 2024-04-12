#include "SceneManager.h"
#include "BaseScene.h"
#include "Renderer.h"
SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::Finalize()
{
	scene_->Finalize();
	delete scene_;
}

void SceneManager::Update()
{

	if (nextScene_) {

		if (!scene_) {
			scene_ = nextScene_;
			nextScene_ = nullptr;
			scene_->SetSceneManager(this);
			scene_->Initialize();
		}

		if (Renderer::GetInstance()->GetIsNextScene()) {
			if (scene_) {
				scene_->Finalize();
				delete scene_;
			}

			scene_ = nextScene_;
			nextScene_ = nullptr;

			scene_->SetSceneManager(this);

			scene_->Initialize();
		}
	}

	scene_->Update();
}

void SceneManager::ModelDraw()
{
	scene_->ModelDraw();
}

void SceneManager::SkyDraw()
{
	scene_->SkyDraw();
}

void SceneManager::ShadowDraw()
{
	scene_->ShadowDraw();
}

void SceneManager::SpotLightShadowDraw()
{
	scene_->SpotLightShadowDraw();
}

void SceneManager::ParticleDraw()
{
	scene_->ParticleDraw();
}

void SceneManager::ParticleBoxDraw()
{
	scene_->ParticleBoxDraw();
}

void SceneManager::PreSpriteDraw()
{
	scene_->PreSpriteDraw();
}

void SceneManager::PostSpriteDraw()
{
	scene_->PostSpriteDraw();
}
