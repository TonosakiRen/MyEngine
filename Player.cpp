#include "Player.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Easing.h"
void Player::Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight)
{
	GameObject::Initialize(name, viewProjection, directionalLight);
	input_ = Input::GetInstance();

	dustParticle_ = std::make_unique<DustParticle>();
	dustParticle_->Initialize({ 0.0f,0.0f,-1.0f }, { 0.0f,1.0f,0.0f });
	dustParticle_->emitterWorldTransform_.SetParent(&worldTransform_);
	//煙の出る場所
	dustParticle_->emitterWorldTransform_.translation_ = { 0.0f,-2.1f,-1.2f };

	material_.enableLighting_ = false;
	worldTransform_.rotation_.y = Radian(90.0f);
	worldTransform_.translation_.y = 6.0f;

	accelaration_ = { 0.0f,-0.03f,0.0f };
	velocity_ = { 0.0f,0.0f,0.0f };
	modelParts_.Initialize("player_part");
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].Initialize();
		partsTransform_[i].SetParent(&worldTransform_);
	}
	//model位置初期化
	{
		partsTransform_[LeftArm].translation_.x = 1.510f;
		partsTransform_[RightArm].translation_.x = -1.510f;
		partsTransform_[LeftArm].scale_ = { 0.4f,0.4f, 0.4f };
		partsTransform_[RightArm].scale_ = { 0.4f,0.4f, 0.4f };

		partsTransform_[RightLeg].translation_.y = 1.470f - 2.79f;
		partsTransform_[LeftLeg].translation_.y = 1.470f - 2.79f;
		partsTransform_[RightLeg].translation_.x = 1.120f;
		partsTransform_[LeftLeg].translation_.x = -1.120f;

		partsTransform_[LeftLeg].scale_ = { 0.4f,0.4f, 0.4f };
		partsTransform_[RightLeg].scale_ = { 0.4f,0.4f, 0.4f };
	}

	collider.Initialize(&worldTransform_, name, viewProjection, directionalLight, { 1.8f,1.72f,1.0f });

	weaponRotateWorldTransform_.UpdateMatrix();
	weaponRotateWorldTransform_.SetParent(&worldTransform_);

	weaponObject_.Initialize(viewProjection, directionalLight);
	weaponObject_.SetScale({ 0.4f,2.0f,0.5f });
	weaponObject_.SetPosition({ 0.0f,4.6f,0.0f });
	weaponObject_.UpdateMatrix();
	weaponObject_.SetParent(&weaponRotateWorldTransform_);
	blockHandle_ = TextureManager::Load("block.png");

	
	weaponCollider_.Initialize(weaponObject_.GetWorldTransform(), "武器", viewProjection, directionalLight);
}

void Player::Update()
{

	if (behaviorRequest_) {
		//振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		//各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kMove:
		default:
			BehaviorMoveInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kDash:
			BehaviorDashInitialize();
			break;
		}
		//振る舞いリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}


	switch (behavior_) {
	case Behavior::kMove:
	default:
		BehaviorMoveUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kDash:
		BehaviorDashUpdate();
		break;
	}

	UpdateMatrix();
}

void Player::UpdateMatrix()
{

	worldTransform_.UpdateMatrix(rotate);

	weaponCollider_.AdjustmentScale();
	weaponRotateWorldTransform_.UpdateMatrix();
	weaponObject_.UpdateMatrix();

	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}
}


void Player::Collision(Collider& blockCollider)
{

	float minOverlap = FLT_MAX;
	Vector3 minAxis = { 0.0f,0.0f,0.0f };

	isGround_ = false;
	if (collider.Collision(blockCollider, minAxis, minOverlap)) {
		float dot = Dot(MakeTranslation(blockCollider.worldTransform_.matWorld_) - MakeTranslation(collider.worldTransform_.matWorld_), minAxis);
		if (dot > 0.0f) {
			minOverlap = -minOverlap;
		}

		isGround_ = true;
		worldTransform_.translation_ += Normalize(minAxis) * minOverlap;
		worldTransform_.UpdateMatrix();

		if (minAxis.y >= 1.0f) {
			velocity_.y = 0.0f;
			worldTransform_.SetParent(blockCollider.worldTransform_.GetParent());
		}
	}

	worldTransform_.UpdateMatrix();
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}

	dustParticle_->SetIsEmit(isGround_ && isWalking_);
	dustParticle_->Update();
}
bool Player::weaponCollision(Collider& bossCollider)
{
	if (isAttack_) {
		bool isHit = false;
		isHit = weaponCollider_.Collision(bossCollider);
		return isHit;
	}
	return false;
}
void Player::Animation() {
	dustParticle_->SetIsEmit(true);
	if (animationT_ >= 1.0f || animationT_ <= 0.0f)
	{
		animationSpeed_ *= -1.0f;
	}

	if (animationBodyT_ >= 1.0f)
	{
		animationBodyT_ = 0.0f;
		runUpAnimation_ *= -1.0f;
	}

	partsTransform_[RightArm].rotation_.x = Easing::easing(animationT_, -0.6f, 0.6f, animationSpeed_, Easing::EasingMode::easeNormal, false);
	partsTransform_[LeftArm].rotation_.x = -partsTransform_[RightArm].rotation_.x;

	partsTransform_[RightLeg].rotation_.x = Easing::easing(animationT_, -0.4f, 0.4f, animationSpeed_, Easing::EasingMode::easeNormal, false);
	partsTransform_[LeftLeg].rotation_.x = -partsTransform_[RightLeg].rotation_.x;

	worldTransform_.rotation_.y += Easing::easing(animationT_, -Radian(10.0f), Radian(10.0f), animationSpeed_, Easing::EasingMode::easeNormal, false);

	worldTransform_.translation_.y += Easing::easing(animationBodyT_, 0.0f, runUpAnimation_, animationBodySpeed_, Easing::EasingMode::easeNormal, false);


	animationT_ += animationSpeed_;
	animationBodyT_ += animationBodySpeed_;
}
void Player::Draw() {
	model_.Draw(worldTransform_, *viewProjection_, *directionalLight_, material_);
	for (int i = 0; i < partNum; i++) {
		modelParts_.Draw(partsTransform_[i], *viewProjection_, *directionalLight_, material_);
	}
	weaponCollider_.Draw();
	weaponObject_.Draw(blockHandle_);
}

void Player::ParticleDraw() {
	dustParticle_->Draw(viewProjection_, directionalLight_, { 0.5f,0.5f,0.5f,1.0f });
}

void Player::SetInitialPos()
{
	worldTransform_.SetParent(nullptr);
	worldTransform_.translation_ = { 0.0f,6.0f,0.0f };
	worldTransform_.UpdateMatrix();
}

void Player::BehaviorMoveUpdate()
{

	if (input_->TriggerKey(DIK_R)) {
		behaviorRequest_ = Behavior::kAttack;
	}

	if (input_->TriggerKey(DIK_LSHIFT)) {
		behaviorRequest_ = Behavior::kDash;
	}

	isGrounding_ = false;

	Vector3 move = { 0.0f,0.0f,0.0f };
	isWalking_ = false;
	if (input_->GetIsGamePadConnect()) {
		// 速さ
		const float speed = 0.3f;
		// 移動量
		move = {
			input_->GetLStick().x / SHRT_MAX, 0.0f,
			input_->GetLStick().y / SHRT_MAX };

		// 移動量に速さを反映
		if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f) {
			move = Normalize(move) * speed;
			isWalking_ = true;
		}
		Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewProjection_->target_.y);
		move = move * rotateMatrix;

		if (input_->GetLStick().x != 0.0f || input_->GetLStick().y != 0.0f) {
			Vector3 nowDirection = Normalize(Vector3{ 0.0f,0.0f,1.0f } *MakeRotateYMatrix(worldTransform_.rotation_.y));

			Matrix4x4 directionMatrix = DirectionToDirection(nowDirection, move);

			worldTransform_.rotation_ = worldTransform_.rotation_ * directionMatrix;

		}

		if (input_->TriggerButton(XINPUT_GAMEPAD_A) && isJump_ == false) {
			isGround_ = false;
			velocity_.y = 0.5f;
		};
	}
	else {
		if (input_->PushKey(DIK_W)) {
			move.z += 0.2f;
			isWalking_ = true;
		}
		if (input_->PushKey(DIK_A)) {
			move.x -= 0.2f;
			isWalking_ = true;
		}
		if (input_->PushKey(DIK_S)) {
			move.z -= 0.2f;
			isWalking_ = true;
		}
		if (input_->PushKey(DIK_D)) {
			move.x += 0.2f;
			isWalking_ = true;
		}

		Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewProjection_->target_.y);
		move = move * rotateMatrix;

		if (move.x != 0.0f || move.z != 0.0f) {

			Vector3 nowDirection = Normalize(Vector3{ 0.0f,0.0f,1.0f } *MakeRotateYMatrix(worldTransform_.rotation_.y));
			rotate = DirectionToDirection(nowDirection, move);
			Vector3 move2 = nowDirection * rotate;
			worldTransform_.rotation_ = worldTransform_.rotation_ * rotate;
		}
		

		if (input_->PushKey(DIK_SPACE) && isJump_ == false) {
			isGround_ = false;
			velocity_.y = 0.5f;
		};
	}

	if (isWalking_) {
		//Animation();
	}

	// 移動
	worldTransform_.translation_ = worldTransform_.translation_ + move;

	velocity_ += accelaration_;
	worldTransform_.translation_ += velocity_;

	if (worldTransform_.translation_.y <= -30.0f) {
		worldTransform_.SetParent(nullptr);
		worldTransform_.translation_ = { 0.0f,3.0f,0.0f };
	}
}

void Player::BehaviorAttackUpdate()
{
	weaponRadian_ += Radian(3.0f);
	
	if (weaponRadian_ >= Radian(90.0f)) {
		behaviorRequest_ = Behavior::kMove;
		weaponRadian_ = 0.0f;
	}

	weaponRotateWorldTransform_.rotation_.x = weaponRadian_;
}

void Player::BehaviorDashUpdate()
{

	dashFrame++;
	// 速さ
	const float speed = 0.9f;

	Matrix4x4 rotateMatrix = MakeRotateYMatrix(worldTransform_.rotation_.y);

	Vector3 move = Normalize(Vector3{ 0.0f,0.0f,1.0f }) * rotateMatrix * speed;

	// 移動
	worldTransform_.translation_ = worldTransform_.translation_ + move;

	if (dashFrame >= 10) {
		behaviorRequest_ = Behavior::kMove;
		dashFrame = 0;
	}
}

void Player::BehaviorMoveInitialize()
{
}

void Player::BehaviorAttackInitialize()
{
}

void Player::BehaviorDashInitialize()
{
}