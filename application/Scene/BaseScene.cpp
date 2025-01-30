/**
 * @file BaseScene.cpp
 * @brief sceneの仮想Class
 */
#include "Scene/BaseScene.h"
#include "Scene/SceneManager.h"
#include "Draw/DrawManager.h"
#include "GameComponent/Collider.h"
#include "ImGui/ImGuiManager.h"


ViewProjection* BaseScene::currentViewProjection;
WavePoints* BaseScene::wavePoints;
std::unique_ptr <DebugCamera> BaseScene::debugCamera_;
std::unique_ptr<WavePoints> BaseScene::wavePoints_;
std::unique_ptr<Camera> BaseScene::camera_;
std::unique_ptr<Player> BaseScene::player_;

void BaseScene::StaticInitialize()
{
	const Vector3 initializeCameraPos = { 0.0f,3.2f,-11.0f };
	const Vector3 initializeCameraRotate = { 0.06f,0.0f,0.0f };

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(initializeCameraPos, initializeCameraRotate);


	camera_ = std::make_unique<Camera>();
	camera_->Initialize(initializeCameraPos, MakeFromEulerAngle(initializeCameraRotate));

	Engine::DrawManager::GetInstance()->SetCallingViewProjection(*camera_);

	wavePoints_ = std::make_unique<WavePoints>();
	wavePoints_->Initialize();
	wavePoints = wavePoints_.get();

	player_ = std::make_unique<Player>();
	player_->Initialize("walk.gltf");
}

void BaseScene::CommonUpdate()
{
	Collider::SwitchIsDrawCollider();
	ViewProjection::SwitchIsUseDebugCamera();
	//fps表示
#ifdef _DEBUG
	ImGui::Begin("fps");
	auto io = ImGui::GetIO();
	ImGui::Text("%f", io.Framerate);
	ImGui::End();
#endif

	wavePoints->Update();
	wavePoints->Draw();

	//camera light
	{
		if (ViewProjection::isUseDebugCamera) {
			currentViewProjection = debugCamera_.get();
			debugCamera_->Update();
		}
		else {
			currentViewProjection = camera_.get();
		}
		if (player_) {
			camera_->Update(MakeTranslation(player_->GetWorldTransform().matWorld_));
		}
		else {
			camera_->Update();
		}

		//camera_->Draw();
	}
}

void BaseScene::StaticFinalize()
{
	player_.reset();
	debugCamera_.reset();
	wavePoints_.reset();
	camera_.reset();
}
