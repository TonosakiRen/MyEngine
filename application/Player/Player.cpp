/**
 * @file Player.h
 * @brief player
 */
#include "Player/Player.h"
#include "ImGuiManager.h"
#include "Player/PlayerBulletManager.h"
#include "Texture/TextureManager.h"
#include "Audio.h"
#include "Render/Wire.h"
#include "Draw/DrawManager.h"
#include "Stage/Floor.h"
#include "Mesh/GlitchMeshletModel.h"


void Player::Initialize(const std::string name)
{
	GameObject::Initialize(name);
	material_.Update();
	input_ = Engine::Input::GetInstance();
	modelSize_ = modelManager->GetModelSize(modelHandle_);
	Vector3 modelCenter = modelManager->GetModelCenter(modelHandle_);
	collider_.Initialize(&worldTransform_, name,modelHandle_);

	velocity_ = { 0.0f,0.0f,0.0f };
	acceleration_ = { 0.0f,-0.05f,0.0f };
	uint32_t handle = Engine::TextureManager::Load("reticle.png");
	sprite2DReticle_.Initialize(handle, { 0.0f,0.0f });
	worldTransform3DReticle_.Initialize();
	direction_ = { 0.0f,0.0f,1.0f };

	playerModel_.Initialize(worldTransform_);
	playerModel_.SetMoving(PlayerModel::kStand);
	playerModel_.Update();
	
	PointLights* pointLights = LightManager::GetInstance()->pointLights_.get();
	//PointLightの初期か
	for (int i = 0; i < PointLights::lightNum; i++) {
		if (pointLights->lights_[i].isActive == false) {
			pointLight_ = &pointLights->lights_[i];
			pointLight_->isActive = true;
			pointLight_->worldTransform.Reset();
			pointLight_->worldTransform.translation_ = modelCenter;
			pointLight_->worldTransform.SetParent(&worldTransform_, false);
			pointLight_->decay = 1.0f;
			pointLight_->intensity = 3.0f;
			pointLight_->radius = 6.0f;
			pointLight_->color = color_;
			break;
		}
	}

	lightCollider_.Initialize(&worldTransform_, name, modelHandle_);
}

void Player::Update(const ViewProjection& viewProjection)
{
	playerModel_.SetMoving(PlayerModel::kStand);
	Move(viewProjection);
	
	ReticleUpdate(viewProjection);
	Fire();

	//光の当たり判定球が膨張していたら
	if (isGrowSphere_) {
		lightSphereT_ += growSpeed;
		lightSphereT_ = Clamp(lightSphereT_, 0.0f, 1.0f);
		const float maxScale = 400.0f;
		lightCollider_.worldTransform_.scale_ = { (maxScale * lightSphereT_),(maxScale * lightSphereT_), (maxScale * lightSphereT_) };
		if (lightSphereT_ >= 1.0f) {
			isGrowSphere_ = false;
			lightSphereT_ = 0.0f;
			lightCollider_.worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
		}
	}

	GlitchMeshletModel::t = lightSphereT_;

#ifdef _DEBUG
	ImGui::Begin("Game");
	if (ImGui::BeginMenu("Player")) {
		ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.1f);
		ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
		ImGui::DragFloat4("color", &color_.x, 0.01f, 0.0f, 1.0f);
		ImGui::EndMenu();
	}
	ImGui::End();
#endif

	//色を1.0fまで遷移
	colorT_ += 0.02f;
	colorT_ = Clamp(colorT_, 0.0f, 1.0f);
	material_.color_ = Lerp(colorT_, material_.color_, color_);
	pointLight_->color = material_.color_;

	material_.Update();
	worldTransform_.Update();
	worldTransform3DReticle_.Update();
	playerModel_.Update();
}

void Player::Extrusion(Collider& otherCollider)
{
	Vector3 pushBackVector;
	if (collider_.Collision(otherCollider, pushBackVector)) {
		worldTransform_.translation_ += pushBackVector;
		worldTransform_.Update();
	}

}

void Player::OnCollision()
{

}

void Player::SetColor(const Vector4& color)
{
	colorT_ = 0.0f;
	color_ = color;
}

void Player::Draw() {
	if (input_->PushRightTrigger()) {
		Engine::DrawManager::GetInstance()->DrawPostSprite(sprite2DReticle_);
	}
	playerModel_.Draw();
	collider_.Draw();
}

void Player::Fire()
{
	if (input_->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) || input_->TriggerKey(DIK_SPACE) || input_->TriggerButton(XINPUT_GAMEPAD_B) || input_->TriggerButton(XINPUT_GAMEPAD_A)) {
		isGrowSphere_ = true;
	}
}


void Player::Move(const ViewProjection& viewProjection)
{
	Vector3 move = { 0.0f,0.0f,0.0f };
	// 速さ
	const float speed = 0.1f;
	if (input_->GetIsGamePadConnect()) {
		// 移動量
		move = {
			input_->GetLStick().x / SHRT_MAX, 0.0f,
			input_->GetLStick().y / SHRT_MAX };

		// 移動量に速さを反映
		if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f) {
			move = Normalize(move) * speed;
		}
	}
	else {
		if (input_->PushKey(DIK_A)) {
			move.x -= speed;
		}
		if (input_->PushKey(DIK_D)) {
			move.x += speed;
		}
		if (input_->PushKey(DIK_S)) {
			move.z -= speed;
		}
		if (input_->PushKey(DIK_W)) {
			move.z += speed;
		}
	}

	Quaternion yq = MakeYAxisFromQuaternion(viewProjection.GetQuaternion());
	move = move * yq;

	//移動していたら
	if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f) {
		direction_ = Normalize(move);
		inputQuaternion_ = MakeLookRotation(direction_);
		worldTransform_.quaternion_ = Slerp(0.2f, worldTransform_.quaternion_, inputQuaternion_);
		playerModel_.SetMoving(PlayerModel::kWalk);
	}

	//ジャンプ
	if (input_->TriggerKey(DIK_SPACE) || input_->TriggerButton(XINPUT_GAMEPAD_A)) {
		//velocity_.y = 0.3f;
	}

	//移動し制限、速度加速度加算
	velocity_.y = Clamp(velocity_.y, -0.5f, 200.0f);
	velocity_ += acceleration_;
	worldTransform_.translation_ += velocity_;
	
	worldTransform_.translation_ += move;
	worldTransform_.translation_.x = Clamp(worldTransform_.translation_.x , -Floor::kFloorHalfSize + modelSize_.x / 2.0f , Floor::kFloorHalfSize - modelSize_.x / 2.0f);
	worldTransform_.translation_.y = Clamp(worldTransform_.translation_.y,0.0f, FLT_MAX);
	worldTransform_.translation_.z = Clamp(worldTransform_.translation_.z, -Floor::kFloorHalfSize + modelSize_.z / 2.0f, FLT_MAX);
}

void Player::ReticleUpdate(const ViewProjection& viewProjection)
{

	POINT mousePosition;
	//マウス座標8(スクリーン座標を取得する)
	GetCursorPos(&mousePosition);

	//クライアントエリア座標に変換する
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);
	Vector2 mousePos = { static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y) };

	//ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	//ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport = viewProjection.GetMatView() * viewProjection.GetMatProjection() * matViewport;

	if (input_->PushRightTrigger()) {
		if (input_->GetIsGamePadConnect()) {
			// スプライトへの現在座標を取得
			Vector2 spritePosition = sprite2DReticle_.position_;

			spritePosition.x += (float)input_->GetRStick().x / SHRT_MAX * 5.0f;
			spritePosition.y -= (float)input_->GetRStick().y / SHRT_MAX * 5.0f;
			//ライトの座標を羽根井
			sprite2DReticle_.position_ = spritePosition;

			// ビュープロジェクションビューポート合成行列
			Matrix4x4 matVPV = matViewProjectionViewport;
			// 合成行列の逆行列を計算する
			Matrix4x4 matInverseVPV = Inverse(matVPV);
			// スクリーン座標
			Vector3 posNear = Vector3(spritePosition.x, spritePosition.y, 0.0f);
			Vector3 posFar = Vector3(spritePosition.x, spritePosition.y, 1.0f);

			// スクリーン座標系からワールド座標系
			posNear = TransformVector(posNear, matInverseVPV);
			posFar = TransformVector(posFar, matInverseVPV);

			// マウスレイの方向
			Vector3 mouseDirection = posFar - posNear;
			mouseDirection = Normalize(mouseDirection);
			// カメラから標準プロジェクトの距離
			const float kDistanceTestObject = 20.0f;
			worldTransform3DReticle_.translation_ = posNear + (mouseDirection * kDistanceTestObject);
			worldTransform3DReticle_.Update();
		}
		else {
			// スプライトのレティクルに座標設定
			sprite2DReticle_.position_ = Vector2(mousePos.x, mousePos.y);

			// ビュープロジェクションビューポート合成行列
			Matrix4x4 matVPV = matViewProjectionViewport;
			// 合成行列の逆行列を計算する
			Matrix4x4 matInverseVPV = Inverse(matVPV);
			// スクリーン座標
			Vector3 posNear = Vector3(mousePos.x, mousePos.y, 0.0f);
			Vector3 posFar = Vector3(mousePos.x, mousePos.y, 1.0f);

			// スクリーン座標系からワールド座標系
			posNear = TransformVector(posNear, matInverseVPV);
			posFar = TransformVector(posFar, matInverseVPV);

			// マウスレイの方向
			Vector3 mouseDirection = posFar - posNear;
			mouseDirection = Normalize(mouseDirection);
			// カメラから標準プロジェクトの距離
			const float kDistanceTestObject = 40.0f;
			worldTransform3DReticle_.translation_ = posNear + (mouseDirection * kDistanceTestObject);
			worldTransform3DReticle_.Update();
		}
	}
	else {
		Vector2 screenCenter = { WinApp::kWindowWidth / 2.0f , WinApp::kWindowHeight / 2.0f };
		// スプライトのレティクルに座標設定
		sprite2DReticle_.position_ = screenCenter;

		// ビュープロジェクションビューポート合成行列
		Matrix4x4 matVPV = matViewProjectionViewport;
		// 合成行列の逆行列を計算する
		Matrix4x4 matInverseVPV = Inverse(matVPV);
		// スクリーン座標
		Vector3 posNear = Vector3(screenCenter.x, screenCenter.y, 0.0f);
		Vector3 posFar = Vector3(screenCenter.x, screenCenter.y, 1.0f);

		// スクリーン座標系からワールド座標系
		posNear = TransformVector(posNear, matInverseVPV);
		posFar = TransformVector(posFar, matInverseVPV);

		// マウスレイの方向
		Vector3 mouseDirection = posFar - posNear;
		mouseDirection = Normalize(mouseDirection);
		// カメラから標準プロジェクトの距離
		const float kDistanceTestObject = 50.0f;
		worldTransform3DReticle_.translation_ = posNear + (mouseDirection * kDistanceTestObject);
		worldTransform3DReticle_.Update();
	}
	

	
}
