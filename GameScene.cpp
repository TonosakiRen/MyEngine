#include "GameScene.h"
#include "externals/imgui/imgui.h"
#include <cassert>

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

	triangle_.Initialize();
	triangleTransform_.Initialize(dxCommon_->GetDevice());
	triangleTransform_.translation_ = { 0.0f,0.0f,0.0f };
	triangleTransform_.UpdateMatrix();

	triangle2_.Initialize();
	triangleTransform2_.Initialize(dxCommon_->GetDevice());
	triangleTransform2_.translation_ = { 0.0f,0.0f,0.0f };
	triangleTransform2_.UpdateMatrix();

	sphere_.Initialize();
	sphereTransform_.Initialize(dxCommon_->GetDevice());
	sphereTransform_.translation_ = { 0.0f,0.0f,0.0f };
	sphereTransform_.UpdateMatrix();

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


}

void GameScene::Update(){

	// camera
	viewProjection_.DebugMove();
	viewProjection_.UpdateMatrix();
	// light
	ImGui::DragFloat3("light", &directionalLight_.direction_.x, 0.01f);
	ImGui::DragFloat4("lightcolor", &directionalLight_.color_.x, 0.01f);
	directionalLight_.UpdateDirectionalLight();
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
	// triangle
	ImGui::DragFloat3("triangleTransform", &triangleTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("triangleRotate", &triangleTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("triangleScale", &triangleTransform_.scale_.x, 0.01f);
	triangleTransform_.UpdateMatrix();
	// triangle2
	ImGui::DragFloat3("triangle2Transform", &triangleTransform2_.translation_.x, 0.01f);
	ImGui::DragFloat3("triangle2Rotate", &triangleTransform2_.rotation_.x, 0.01f);
	ImGui::DragFloat3("triangle2Scale", &triangleTransform2_.scale_.x, 0.01f);
	triangleTransform_.UpdateMatrix();

	// sphere
	ImGui::DragFloat3("sphereTransform", &sphereTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("sphereRotate", &sphereTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("sphereScale", &sphereTransform_.scale_.x, 0.01f);
	sphereTransform_.UpdateMatrix();

	ImGui::DragFloat2("spriteTransform", &spritePosition_.x, 1.0f);
	ImGui::DragFloat("spriteRotate", &spriteRotate_, 1.0f);
	ImGui::DragFloat2("spriteScale", &spriteScale_.x, 1.0f);
	sprite_.SetPosition(spritePosition_);
	sprite_.SetRotation(spriteRotate_);
	sprite_.SetSize(spriteScale_);
	
}

void GameScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// 
	sprite_.Draw();

	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

	//深度バッファクリア
	dxCommon_->ClearDepthBuffer();

	//3Dオブジェクト描画前処理
	Triangle::PreDraw(commandList);
	Model::PreDraw(commandList);
	Sphere::PreDraw(commandList);

	model_.Draw(modelTransform_, viewProjection_, directionalLight_, material_, textureHandle_);
	triangle_.Draw(triangleTransform_, viewProjection_, directionalLight_, material_, textureHandle_);
	triangle2_.Draw(triangleTransform2_, viewProjection_, directionalLight_, material_, textureHandle_);
	sphere_.Draw(sphereTransform_, viewProjection_, directionalLight_, material_, textureHandle_);

	//3Dオブジェクト描画後処理
	Model::PostDraw();
	Triangle::PostDraw();
	Sphere::PostDraw();
}