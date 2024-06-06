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

ViewProjection* GameScene::currentViewProjection_ = nullptr;
DirectionalLights* GameScene::directionLights = nullptr;
PointLights* GameScene::pointLights = nullptr;
AreaLights* GameScene::areaLights = nullptr;
SpotLights* GameScene::spotLights = nullptr;
ShadowSpotLights* GameScene::shadowSpotLights = nullptr;

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

	directionalLights_ = std::make_unique<DirectionalLights>();
	directionalLights_->Initialize();
	directionalLights_->Update();

	pointLights_ = std::make_unique<PointLights>();
	pointLights_->Initialize();

	areaLights_ = std::make_unique<AreaLights>();
	areaLights_->Initialize();

	shadowSpotLights_ = std::make_unique<ShadowSpotLights>();
	shadowSpotLights_->Initialize();
	shadowSpotLights_->Update();

	spotLights_ = std::make_unique<SpotLights>();
	spotLights_->Initialize();
	spotLights_->Update();

	currentViewProjection_ = debugCamera_.get();
	directionLights = directionalLights_.get();
	pointLights = pointLights_.get();
	areaLights = areaLights_.get();
	spotLights = spotLights_.get();
	shadowSpotLights = shadowSpotLights_.get();

	compute_ = std::make_unique<Compute>();
	compute_->Initialize();

	sceneManager_ = SceneManager::GetInstance();
	BaseScene* scene = new GamePlayScene();
	sceneManager_->SetNextScene(scene);

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
	//camera light
	{
		if (ViewProjection::isUseDebugCamera) {
			currentViewProjection_ = debugCamera_.get();
			debugCamera_->Update();
		}
		else {
			currentViewProjection_ = camera_.get();
			if (GamePlayScene::player) {
				camera_->Update(MakeTranslation(GamePlayScene::player->GetWorldTransform().matWorld_));
			}
			else {
				camera_->Update();
			}
		}



		
		// light
#ifdef USE_IMGUI
		ImGui::Begin("DirectionalLight");
		ImGui::DragFloat3("lightDirection", &directionalLights_->lights_[0].direction.x, 0.01f);
		ImGui::DragFloat3("lightPosition", &directionalLights_->lights_[0].position.x, 1.0f);
		ImGui::DragFloat4("lightColor", &directionalLights_->lights_[0].color.x, 1.0f,0.0f,255.0f);
		ImGui::DragFloat("intensity", &directionalLights_->lights_[0].intensity, 0.01f, 0.0f);
		ImGui::End();
#endif
		directionalLights_->lights_[0].direction = Normalize(directionalLights_->lights_[0].direction);
		directionalLights_->Update();

#ifdef USE_IMGUI
		ImGui::Begin("spotLight");
		ImGui::DragFloat3("lightPosition", &spotLights_->lights_[0].worldTransform.translation_.x, 0.01f);
		ImGui::DragFloat3("lightColor", &spotLights_->lights_[0].color.x, 1.0f, 0.0f, 255.0f);
		ImGui::DragFloat("intensity", &spotLights_->lights_[0].intensity, 0.01f, 0.0f);
		ImGui::DragFloat3("direction", &spotLights_->lights_[0].direction.x, 0.01f, 0.0f);
		ImGui::DragFloat("distance", &spotLights_->lights_[0].distance, 0.01f, 0.0f);
		ImGui::DragFloat("decay", &spotLights_->lights_[0].decay, 0.01f, 0.0f);
		ImGui::DragFloat("cosAngle", &spotLights_->lights_[0].cosAngle, Radian(1.0f), 0.0f, Radian(179.0f));
		ImGui::End();

		ImGui::Begin("areaLights");
		ImGui::DragFloat3("origin", &areaLights->lights_[0].segment.origin.x);
		ImGui::DragFloat3("diff", &areaLights->lights_[0].segment.diff.x);
		ImGui::DragFloat3("lightColor", &areaLights->lights_[0].color.x, 1.0f, 0.0f, 255.0f);
		ImGui::DragFloat("intensity", &areaLights->lights_[0].intensity, 0.01f, 0.0f);
		ImGui::DragFloat("range", &areaLights->lights_[0].range, 0.01f, 0.0f);
		ImGui::DragFloat("decay", &areaLights->lights_[0].decay, 0.01f, 0.0f);
		ImGui::End();

		areaLights_->Update();

#endif
		areaLights_->Update();
		spotLights_->lights_[0].direction = Normalize(spotLights_->lights_[0].direction);
		spotLights_->Update();
	}	

	sceneManager_->Update();
		

	//コンピュートシェーダテスト
	{
		compute_->Dispatch(commandContext);
		uint32_t* date = static_cast<uint32_t*>(compute_->GetData());

		int a = date[2];
#ifdef _DEBUG
		ImGui::Text("%d", int(a));
		#endif
	}

}

void GameScene::Draw() {
	sceneManager_->Draw();
}
