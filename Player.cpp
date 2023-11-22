#include "Player.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Easing.h"
#include "GlobalVariables.h"
#include "LockOn.h"

std::array<Player::ConstAttack, Player::ComboNum> Player::kConstAttacks_ = {
	{
		{0,Radian(3.0f)},
		{5,Radian(3.0f)},
		{15,Radian(3.0f)}
	}
};

void Player::ApplyGlobalVariables() {

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	dashSpeed_ = globalVariables->GetFloatValue(groupName, "dashSpeed");
	kConstAttacks_[ShortSwing].AttackSpeed = globalVariables->GetFloatValue(groupName, "shortSwingAttackSpeed");
	kConstAttacks_[Drop].AttackSpeed = globalVariables->GetFloatValue(groupName, "dropAttackSpeed");
	kConstAttacks_[LongSwing].AttackSpeed = globalVariables->GetFloatValue(groupName, "longSwingAttackSpeed");
}

void Player::Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight)
{

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "dashSpeed", dashSpeed_);
	globalVariables->AddItem(groupName, "shortSwingAttackSpeed", kConstAttacks_[ShortSwing].AttackSpeed);
	globalVariables->AddItem(groupName, "dropAttackSpeed", kConstAttacks_[Drop].AttackSpeed);
	globalVariables->AddItem(groupName, "longSwingAttackSpeed", kConstAttacks_[LongSwing].AttackSpeed);

	ApplyGlobalVariables();

	GameObject::Initialize(name, viewProjection, directionalLight);
	input_ = Input::GetInstance();

	dustParticle_ = std::make_unique<DustParticle>();
	dustParticle_->Initialize({ 0.0f,0.0f,-1.0f }, { 0.0f,1.0f,0.0f });
	dustParticle_->emitterWorldTransform_.SetParent(&worldTransform_);
	//煙の出る場所
	dustParticle_->emitterWorldTransform_.translation_ = { 0.0f,-2.1f,-1.2f };

	material_.enableLighting_ = false;
	worldTransform_.quaternion_ *= MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, Radian(90.0f));
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
	ApplyGlobalVariables();
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
	worldTransform_.UpdateMatrix();

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
	if (behavior_ == Behavior::kAttack && isHited_ == false) {
		bool isHit = false;
		isHit = weaponCollider_.Collision(bossCollider);
		if (isHit) {
			isHited_ = true;
		}
		return isHit;
	}
	return false;
}
void Player::Animation() {
	/*dustParticle_->SetIsEmit(true);
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
	animationBodyT_ += animationBodySpeed_;*/
}
void Player::Draw() {
	model_.Draw(worldTransform_, *viewProjection_, *directionalLight_, material_);
	for (int i = 0; i < partNum; i++) {
		modelParts_.Draw(partsTransform_[i], *viewProjection_, *directionalLight_, material_);
	}
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

	if (input_->TriggerKey(DIK_R) || input_->PushBotton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		behaviorRequest_ = Behavior::kAttack;
	}

	if (input_->TriggerKey(DIK_LSHIFT) || input_->PushBotton(XINPUT_GAMEPAD_LEFT_SHOULDER)) {
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
		else {
			move = { 0.0f,0.0f,0.0f };
		}
		Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewProjection_->target_.y);
		move = move * rotateMatrix;

		if (isWalking_ == true) {

			Vector3 nowDirection = Normalize(Vector3{ 0.0f,0.0f,1.0f } *worldTransform_.quaternion_);

			Vector3 afterDirection = Normalize(move);

			bool isRight = false;

			float a = Cross(Vector2{ nowDirection.x,nowDirection.z }, Vector2{ afterDirection.x,afterDirection.z });

			float angle = Angle(nowDirection, move);

			Quaternion quaternion = MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, angle);

			if (Cross(Vector2{ nowDirection.x,nowDirection.z }, Vector2{ afterDirection.x,afterDirection.z }) < 0.0f) {
				isRight = true;
			}

			if (isRight) {
				quaternion = MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, angle);
			}
			else {
				quaternion = MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, -angle);
			}


			worldTransform_.quaternion_ = worldTransform_.quaternion_ * quaternion;
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

			Vector3 nowDirection = Normalize(Vector3{ 0.0f,0.0f,1.0f } *worldTransform_.quaternion_);

			Vector3 afterDirection = Normalize(move);

			bool isRight = false;

			float angle = Angle(nowDirection, move);

			Quaternion quaternion = MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, angle);

			if (Cross(Vector2{ nowDirection.x,nowDirection.z }, Vector2{ afterDirection.x,afterDirection.z }) < 0.0f) {
				isRight = true;
			}

			if (isRight) {
				quaternion = MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, angle);
			}
			else {
				quaternion = MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, -angle);
			}


			worldTransform_.quaternion_ = worldTransform_.quaternion_ * quaternion;
		}
		else if (lockOn_ && lockOn_->ExitTarget()) {
			Vector3 lockOnPosition = lockOn_->GetTargetPosition();
			Vector3 sub = lockOn_->GetTargetPosition() - MakeTranslation(worldTransform_.matWorld_);

			Vector3 nowDirection = Normalize(Vector3{ 0.0f,0.0f,1.0f } *worldTransform_.quaternion_);

			Vector3 afterDirection = Normalize(Vector3{ sub.x,0.0f,sub.z });

			bool isRight = false;

			float angle = Angle(nowDirection, afterDirection);

			Quaternion quaternion = MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, angle);

			if (Cross(Vector2{ nowDirection.x,nowDirection.z }, Vector2{ afterDirection.x,afterDirection.z }) < 0.0f) {
				isRight = true;
			}

			if (isRight) {
				quaternion = MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, angle);
			}
			else {
				quaternion = MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, -angle);
			}

			worldTransform_.quaternion_ = worldTransform_.quaternion_ * quaternion;
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
		isDrop_ = true;
	}
}

void Player::BehaviorAttackUpdate()
{

	switch (workAttack_.comboPhase)
	{
	case ShortSwing:
		if (workAttack_.initialize == false) {
			weaponRotateWorldTransform_.quaternion_ *= MakeRotateAxisAngleQuaternion({ 1.0f,0.0f,0.0f }, Radian(90.0f));
			weaponRotateWorldTransform_.quaternion_ *= MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, -Radian(30.0f));
			isHited_ = false;
			workAttack_.initialize = true;
		}
		weaponRadian_ += kConstAttacks_[ShortSwing].AttackSpeed;
		weaponRotateWorldTransform_.quaternion_ *= MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, kConstAttacks_[ShortSwing].AttackSpeed);
		if (weaponRadian_ >= Radian(60.0f)) {
			workAttack_.comboFinish = true;
		}
		break;
	case Drop:
		if (workAttack_.initialize == false) {
			workAttack_.initialize = true;
			isHited_ = false;
		}
		weaponRadian_ += kConstAttacks_[Drop].AttackSpeed;
		weaponRotateWorldTransform_.quaternion_ *= MakeRotateAxisAngleQuaternion({ 1.0f,0.0f,0.0f }, kConstAttacks_[Drop].AttackSpeed);
		if (weaponRadian_ >= Radian(90.0f)) {
			workAttack_.comboFinish = true;
		}
		break;
	case LongSwing:
		if (workAttack_.initialize == false) {
			weaponRotateWorldTransform_.quaternion_ *= MakeRotateAxisAngleQuaternion({ 1.0f,0.0f,0.0f }, Radian(90.0f));
			weaponRotateWorldTransform_.quaternion_ *= MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, -Radian(90.0f));
			workAttack_.initialize = true;
			isHited_ = false;
		}
		weaponRadian_ += kConstAttacks_[LongSwing].AttackSpeed;
		weaponRotateWorldTransform_.quaternion_ *= MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, kConstAttacks_[LongSwing].AttackSpeed);
		if (weaponRadian_ >= Radian(360.0f)) {
			workAttack_.comboFinish = true;
		}
		break;
	default:
		break;
	}
	
	if ((input_->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) || input_->TriggerKey(DIK_R)) && workAttack_.comboPhase < ComboNum - 1) {
		workAttack_.comboRequest = static_cast<Combo>(workAttack_.comboPhase + 1);
	}

	if (workAttack_.comboFinish == true) {
		workAttack_.comboFinish = false;

		//攻撃変数初期化
		weaponRadian_ = 0.0f;
		workAttack_.initialize = false;
		weaponRotateWorldTransform_.quaternion_ = { 0.0f,0.0f,0.0f,1.0f };

		//コンボ遷移
		if (workAttack_.comboPhase != workAttack_.comboRequest) {
			workAttack_.comboPhase = workAttack_.comboRequest;
		}
		else {
			workAttack_.comboPhase = ShortSwing;
			workAttack_.comboRequest = ShortSwing;
			behaviorRequest_ = Behavior::kMove;
		}
	}

	if (lockOn_ && lockOn_->ExitTarget()) {
		Vector3 lockOnPosition = lockOn_->GetTargetPosition();
		Vector3 sub = lockOn_->GetTargetPosition() - MakeTranslation(worldTransform_.matWorld_);

		Vector3 nowDirection = Normalize(Vector3{ 0.0f,0.0f,1.0f } *worldTransform_.quaternion_);

		Vector3 afterDirection = Normalize(Vector3{ sub.x,0.0f,sub.z });

		bool isRight = false;

		float angle = Angle(nowDirection, afterDirection);

		Quaternion quaternion = MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, angle);

		if (Cross(Vector2{ nowDirection.x,nowDirection.z }, Vector2{ afterDirection.x,afterDirection.z }) < 0.0f) {
			isRight = true;
		}

		if (isRight) {
			quaternion = MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, angle);
		}
		else {
			quaternion = MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, -angle);
		}

		worldTransform_.quaternion_ = worldTransform_.quaternion_ * quaternion;
	}
	
}

void Player::BehaviorDashUpdate()
{

	dashFrame++;
	
	Vector3 move = Normalize(Vector3{ 0.0f,0.0f,1.0f }) * worldTransform_.quaternion_ * dashSpeed_;

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
