/**
 * @file SceneManager.h
 * @brief Sceneの管理
 */
#include "Scene/SceneManager.h"
#include "Scene/BaseScene.h"
#include "Render/Renderer.h"
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

void SceneManager::Draw()
{
	scene_->Draw();
}