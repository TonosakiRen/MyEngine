#include "GameScene.h"
#include <imgui.h>
#include <cassert>
#include "CommandContext.h"
#include "Renderer.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "ShadowMap.h"
#include "SpotLightShadowMap.h"
#include "WinApp.h"
#include "GamePlayScene.h"
#include "Collider.h"
#include "Particle.h"
#include "ParticleModel.h"
#include "Player.h"
#include "TitleScene.h"
#include "Skinning.h"
#include "DrawManager.h"

ViewProjection* GameScene::currentViewProjection = nullptr;
WavePoints* GameScene::wavePoints = nullptr;

GameScene::GameScene() {};

GameScene::~GameScene() {};

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	Vector3 initializeCameraPos = { 0.0f,3.2f,-11.0f };
	Vector3 initializeCameraRotate = { 0.06f,0.0f,0.0f };

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(initializeCameraPos, initializeCameraRotate);


	camera_ = std::make_unique<Camera>();
	camera_->Initialize(initializeCameraPos, MakeFromEulerAngle(initializeCameraRotate));

	
	compute_ = std::make_unique<Compute>();
	compute_->Initialize();

	sceneManager_ = SceneManager::GetInstance();
	BaseScene* scene = new GamePlayScene();
	sceneManager_->SetNextScene(scene);

	DrawManager::GetInstance()->SetCallingViewProjection(*camera_);
	lightManager_ = LightManager::GetInstance();
	lightManager_->Initialize();

	wavePoints_ = std::make_unique<WavePoints>();
	wavePoints_->Initialize();
	wavePoints = wavePoints_.get();
}

void GameScene::Update(CommandContext& commandContext){
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
		if (GamePlayScene::player) {
			camera_->Update(MakeTranslation(GamePlayScene::player->GetWorldTransform().matWorld_));
		}
		else {
			camera_->Update();
		}

		//camera_->Draw();

		
		// light
#ifdef USE_IMGUI
		ImGui::Begin("Game");
		if (ImGui::BeginMenu("DirectionalLight")) {
			ImGui::DragFloat3("lightDirection", &lightManager_->directionalLights_->lights_[0].direction.x, 0.01f);
			ImGui::DragFloat3("lightPosition", &lightManager_->directionalLights_->lights_[0].position.x, 1.0f);
			ImGui::DragFloat4("lightColor", &lightManager_->directionalLights_->lights_[0].color.x, 1.0f, 0.0f, 255.0f);
			ImGui::DragFloat("intensity", &lightManager_->directionalLights_->lights_[0].intensity, 0.01f, 0.0f);
			ImGui::EndMenu();
		}
#endif
		lightManager_->directionalLights_->lights_[0].direction = Normalize(lightManager_->directionalLights_->lights_[0].direction);

#ifdef USE_IMGUI
		if (ImGui::BeginMenu("areaLights")) {
			ImGui::DragFloat3("origin", &lightManager_->areaLights_->lights_[0].segment.origin.x);
			ImGui::DragFloat3("diff", &lightManager_->areaLights_->lights_[0].segment.diff.x);
			ImGui::DragFloat3("lightColor", &lightManager_->areaLights_->lights_[0].color.x, 1.0f, 0.0f, 255.0f);
			ImGui::DragFloat("intensity", &lightManager_->areaLights_->lights_[0].intensity, 0.01f, 0.0f);
			ImGui::DragFloat("range", &lightManager_->areaLights_->lights_[0].range, 0.01f, 0.0f);
			ImGui::DragFloat("decay", &lightManager_->areaLights_->lights_[0].decay, 0.01f, 0.0f);
			ImGui::EndMenu();
		};
		ImGui::End();

#endif

	}	

	sceneManager_->Update();
	lightManager_->Update();

}

void GameScene::Draw() {
	sceneManager_->Draw();
}