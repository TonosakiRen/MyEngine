#include "GameScene.h"
#include "externals/imgui/imgui.h"
#include <cassert>

#include "ImGuiManager.h"

using namespace DirectX;

GameScene::GameScene() {};

GameScene::~GameScene() {};

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	
	model_.Initialize();
	modelTransform_.Initialize(dxCommon_->GetDevice());
	modelTransform_.translation_ = { 0.0f,0.0f,0.0f };
	modelTransform_.UpdateMatrix();

	model2_.Initialize();
	model2Transform_.Initialize(dxCommon_->GetDevice());
	model2Transform_.translation_ = { 0.0f,0.0f,0.0f };
	model2Transform_.UpdateMatrix();

	/*sphere_.Initialize();
	sphereTransform_.Initialize(dxCommon_->GetDevice());
	sphereTransform_.translation_ = { 0.0f,0.0f,0.0f };
	sphereTransform_.UpdateMatrix();*/

	viewProjection_.Initialize(dxCommon_->GetDevice());

	directionalLight_.Initialize(dxCommon_->GetDevice());
	directionalLight_.direction_ = { 1.0f, -1.0f, 1.0f };
	directionalLight_.UpdateDirectionalLight();

	material_.Initialize(dxCommon_->GetDevice());
	material_.color_ = { 1.0f,1.0f,1.0f,1.0f };
	texTransfrom = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	material_.uvTransform_ = MakeAffineMatrix(texTransfrom.scale, texTransfrom.rotate, texTransfrom.translate);
	material_.UpdateMaterial();

	textureHandle_ = ShaderResourceManager::Load("uvChecker.png");

	spritePosition_ = { 0.0f,0.0f };
	spriteScale_ = { 100.0f,100.0f };
	spriteRotate_ = 0.0f;
	sprite_ = *Sprite::Create(textureHandle_,{0.0f,0.0f});

	shadowCamera.Initialize(dxCommon_->GetDevice());
	lightPos = -directionalLight_.direction_ * 100;
	shadowCamera.constMap->view = Inverse(MakeLookRotation(directionalLight_.direction_) * MakeTranslateMatrix(lightPos));
	shadowCamera.constMap->projection = MakePerspectiveFovMatrix(0.785398f, 1.0f, 1.0f, 1000.0f);
}

void GameScene::Update(){

	// camera
	viewProjection_.DebugMove();
	viewProjection_.UpdateMatrix();
	// light
	ImGui::DragFloat3("light", &directionalLight_.direction_.x, 0.01f);
	ImGui::DragFloat4("lightcolor", &directionalLight_.color_.x, 0.01f);
	directionalLight_.UpdateDirectionalLight();
	//shadowLight
	ImGui::DragFloat3("slight", &lightPos.x, 0.01f);
	lightPos = Normalize( -directionalLight_.direction_) * 100;
	shadowCamera.constMap->view = Inverse(MakeLookRotation(directionalLight_.direction_) * MakeTranslateMatrix(lightPos));
	shadowCamera.constMap->projection = MakePerspectiveFovMatrix(0.785398f, 1.0f, 1.0f, 1000.0f);

	// texture
	ImGui::DragFloat2("texScale", &texTransfrom.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat("texRotate", &texTransfrom.rotate.z, 0.01f);
	ImGui::DragFloat2("texTranslate", &texTransfrom.translate.x, 0.01f, -10.0f, 10.0f);
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(texTransfrom.scale);
	uvTransformMatrix = uvTransformMatrix * MakeRotateZMatrix(texTransfrom.rotate.z);
	uvTransformMatrix = uvTransformMatrix * MakeTranslateMatrix(texTransfrom.translate);
	material_.uvTransform_ = uvTransformMatrix;
	material_.UpdateMaterial();

	// model
	ImGui::DragFloat3("modelTransform", &modelTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("modelRotate", &modelTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("modelScale", &modelTransform_.scale_.x, 0.01f);
	modelTransform_.UpdateMatrix();

	// model
	ImGui::DragFloat3("model2Transform", &model2Transform_.translation_.x, 0.01f);
	ImGui::DragFloat3("model2Rotate", &model2Transform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("model2Scale", &model2Transform_.scale_.x, 0.01f);
	model2Transform_.UpdateMatrix();

	ImGui::Begin("shadow");
	ImTextureID shadowID = reinterpret_cast<ImTextureID>(ShaderResourceManager::GetInstance()->GetGPUDescriptorHandle(ShaderResourceManager::ShadowMapSrv).ptr);
	ImGui::Image(shadowID, { 300.0f,300.0f });
	ImGui::End();

	// sphere
	//ImGui::DragFloat3("sphereTransform", &sphereTransform_.translation_.x, 0.01f);
	//ImGui::DragFloat3("sphereRotate", &sphereTransform_.rotation_.x, 0.01f);
	//ImGui::DragFloat3("sphereScale", &sphereTransform_.scale_.x, 0.01f);
	//sphereTransform_.UpdateMatrix();

	
}

void GameScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	//3Dオブジェクト描画前処理
	Model::BeginPass(commandList, Model::ShadowPass);

	model_.DrawShadow(modelTransform_, shadowCamera);

	//3Dオブジェクト描画後処理
	Model::EndPass();

	////3Dオブジェクト描画前処理
	Sphere::BeginPass(commandList, Sphere::ShadowPass);

	//sphere_.DrawShadow(sphereTransform_, shadowCamera);

	////3Dオブジェクト描画後処理
	Sphere::EndPass();

	
	

	// 描画開始
	dxCommon_->PreDraw();

	//3Dオブジェクト描画前処理
	Model::BeginPass(commandList, Model::DefaultPass);

	model_.Draw(modelTransform_,viewProjection_,directionalLight_,material_,shadowCamera);
	model2_.Draw(model2Transform_, viewProjection_, directionalLight_, material_, shadowCamera);

	//3Dオブジェクト描画後処理
	Model::EndPass();

	////3Dオブジェクト描画前処理
	//Sphere::BeginPass(commandList, Sphere::DefaultPass);

	//sphere_.Draw(sphereTransform_, viewProjection_, directionalLight_, material_, shadowCamera);

	////3Dオブジェクト描画後処理
	//Sphere::EndPass();

	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// 
	

	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

	//深度バッファクリア
	dxCommon_->ClearDepthBuffer();

	// ImGui描画
	ImGuiManager::GetInstance()->Draw();
	// 描画終了
	dxCommon_->PostDraw();
}