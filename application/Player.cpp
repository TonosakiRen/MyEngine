#include "Player.h"
#include "ImGuiManager.h"
#include "ModelManager.h"
#include "PlayerBulletManager.h"
#include "TextureManager.h"
#include "Audio.h"

void Player::Initialize(const std::string name, PlayerBulletManager* playerBulletManager)
{
	GameObject::Initialize(name);
	playerBulletManager_ = playerBulletManager;
	input_ = Input::GetInstance();
	modelSize_ = ModelManager::GetInstance()->GetModelSize(modelHandle_);
	Vector3 modelCenter = ModelManager::GetInstance()->GetModelCenter(modelHandle_);
	collider_.Initialize(&worldTransform_, name,modelSize_, modelCenter);
	worldTransform_.translation_ = { 0.0f,modelSize_.y / 2.0f,0.0f };
	modelWorldTransform_.Initialize();
	modelWorldTransform_.SetParent(&worldTransform_);
	//modelの中心からmodelの高さの半分したにmodelWorldTransformを配置
	modelWorldTransform_.translation_ = { 0.0f,0.0f,0.0f };

	animation_ = AnimationManager::GetInstance()->Load("AnimatedCube.gltf");
	animationTime_ = 0.0f;

	velocity_ = { 0.0f,0.0f,0.0f };
	acceleration_ = { 0.0f,-0.05f,0.0f };
	uint32_t handle = TextureManager::Load("reticle.png");
	sprite2DReticle_.Initialize(handle, { 0.0f,0.0f });
	worldTransform3DReticle_.Initialize();

	uint32_t soundHandle = Audio::GetInstance()->SoundLoadWave("walk.mp3");
	uint32_t playHandle = Audio::GetInstance()->SoundPlayLoopStart(soundHandle);
}

void Player::Update(const ViewProjection& viewProjection)
{

	Move(viewProjection);
	Animate();
	ReticleUpdate(viewProjection);
	Fire();


#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.1f);
	//ImGui::DragFloat3("rotation", &worldTransform_.quaternion_.x, 0.1f);
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat4("color", &color_.x, 0.01f, 0.0f, 1.0f);
	ImGui::End();
#endif

	collider_.AdjustmentScale();
	worldTransform_.Update();
	modelWorldTransform_.Update();
	worldTransform3DReticle_.Update();
}

void Player::Extrusion(Collider& otherCollider)
{
	Vector3 pushBackVector;
	if (collider_.Collision(otherCollider, pushBackVector)) {
		worldTransform_.translation_ += pushBackVector;
		worldTransform_.Update();
		modelWorldTransform_.Update();
	}

}

void Player::OnCollision()
{

}

void Player::Draw() {
	collider_.Draw();
	GameObject::Draw(modelWorldTransform_, {0.0f,0.55f,1.0f,1.0f});
	//GameObject::Draw(modelHandle_,worldTransform3DReticle_, { 0.0f,1.0f,0.0f,1.0f });
}

void Player::ReticleDraw()
{
	if (input_->PushRightTrigger()) {
		sprite2DReticle_.Draw();
	}
}

void Player::Fire()
{
	if (input_->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) || input_->TriggerKey(DIK_E)) {
		Vector3 position = MakeTranslation(worldTransform_.matWorld_);
		Vector3 direction = Normalize(worldTransform3DReticle_.translation_ - MakeTranslation(worldTransform_.matWorld_));
		playerBulletManager_->PopPlayerBullet(position, direction);
	}
}

void Player::Animate()
{
	animationTime_ += 1.0f / 60.0f;
	animationTime_ = std::fmod(animationTime_, animation_.duration);
	NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[ModelManager::GetInstance()->GetRootNode(modelHandle_).name];
	modelWorldTransform_.translation_ = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
	modelWorldTransform_.quaternion_ = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
	modelWorldTransform_.scale_ = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
	
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

	if (input_->TriggerKey(DIK_SPACE) || input_->TriggerButton(XINPUT_GAMEPAD_A)) {
		velocity_.y = 0.3f;
	}

	velocity_.y = clamp(velocity_.y, -0.5f, 200.0f);
	velocity_ += acceleration_;
	worldTransform_.translation_ += velocity_;

	static float cycle = 60.0f;
	static float time = 0.0f;
	// 1フレームでのパラメータ加算値
	const float kFroatStep = 2.0f * std::numbers::pi_v<float> / cycle;
	// パラメータを1ステップ分加算
	time += kFroatStep;
	// 2πを超えたら0に戻すw
	time = std::fmod(time, 2.0f * std::numbers::pi_v<float>);
	// 浮遊を座標に反映
	worldTransform_.translation_.y = 3.0f + (std::sin(time) * 0.3f);
	
	worldTransform_.translation_ += move;
	worldTransform_.translation_.x = clamp(worldTransform_.translation_.x , -25.0f + modelSize_.x / 2.0f , 25.0f - modelSize_.x / 2.0f);
	worldTransform_.translation_.y = clamp(worldTransform_.translation_.y, modelSize_.y / 2.0f, FLT_MAX);
	worldTransform_.translation_.z = clamp(worldTransform_.translation_.z, -25.0f + modelSize_.z / 2.0f, 25.0f - modelSize_.z / 2.0f);
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
			posNear = Transform(posNear, matInverseVPV);
			posFar = Transform(posFar, matInverseVPV);

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
			posNear = Transform(posNear, matInverseVPV);
			posFar = Transform(posFar, matInverseVPV);

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
		posNear = Transform(posNear, matInverseVPV);
		posFar = Transform(posFar, matInverseVPV);

		// マウスレイの方向
		Vector3 mouseDirection = posFar - posNear;
		mouseDirection = Normalize(mouseDirection);
		// カメラから標準プロジェクトの距離
		const float kDistanceTestObject = 50.0f;
		worldTransform3DReticle_.translation_ = posNear + (mouseDirection * kDistanceTestObject);
		worldTransform3DReticle_.Update();
	}
	

	
}
