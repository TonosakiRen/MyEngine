#include "Particle/WhiteParticle.h"
#include "ImGuiManager.h"
#include "Draw/DrawManager.h"

WhiteParticle::WhiteParticle()
{
	particleData_ = std::make_unique<ParticleData>(kParticleNum);
}

void WhiteParticle::Initialize(Vector3 minDirection, Vector3 maxDirection)
{

	particleData_->Initialize();
	emitterWorldTransform_.SetIsScaleParent(false);
	emitterWorldTransform_.Update();
	SetDirection(minDirection, maxDirection);
	emitBox_ = MakeOBB(emitterWorldTransform_.matWorld_);
	emitBox_.size = { 1.0f,1.0f,1.0f };
	material_.Initialize();
	material_.enableLighting_ = false;
	material_.Update();
}

void WhiteParticle::Update() {


	if (isEmit_) {
		for (size_t i = 0; i < EmitNum_; i++) {
			for (size_t j = 0; j < kParticleNum; j++) {
				if (particles[j].isActive == false) {
					particles[j].isActive = true;

					emitterWorldTransform_.Update();
					emitBox_ = MakeOBB(emitterWorldTransform_.matWorld_);

					if (emitterWorldTransform_.GetParent()) {
						particles[j].direction = Normalize(Vector3{ Rand(minDirection_.x, maxDirection_.x) ,Rand(minDirection_.y,maxDirection_.y) ,Rand(minDirection_.z,maxDirection_.z) } *NormalizeMakeRotateMatrix(emitterWorldTransform_.GetParent()->matWorld_));
					}
					else {
						particles[j].direction = Normalize(Vector3{ Rand(minDirection_.x, maxDirection_.x) ,Rand(minDirection_.y,maxDirection_.y) ,Rand(minDirection_.z,maxDirection_.z) });
					}
					particles[j].worldTransform.translation_ = MakeRandVector3(emitBox_);
					particles[j].worldTransform.quaternion_ = IdentityQuaternion();
					particles[j].worldTransform.scale_ = emitterWorldTransform_.scale_;
					break;
				}
			}
		}
	}

	for (size_t i = 0; i < kParticleNum; i++) {
		float rotationSpeed = Radian(2.0f) * (float(i % 2) * 2.0f - 1.0f);
		if (particles[i].isActive == true) {
			particles[i].worldTransform.quaternion_ *= MakeFromAngleAxis({ 0.0f,0.0f,1.0f }, rotationSpeed);
			particles[i].worldTransform.translation_ += particles[i].direction * speed_;
			particles[i].worldTransform.scale_ = particles[i].worldTransform.scale_ - scaleSpeed_;
			particles[i].worldTransform.Update();
			if (particles[i].worldTransform.scale_.x <= 0.0f) {
				particles[i].isActive = false;
			}
		}

	}

}

void WhiteParticle::Draw()
{

	emitterWorldTransform_.Update();

	for (size_t i = 0; i < kParticleNum; i++)
	{
		if (particles[i].isActive) {
			ParticleData::Data data;
			data.matWorld = particles[i].worldTransform.matWorld_;
			data.worldInverseTranspose = Inverse(particles[i].worldTransform.matWorld_);
			particleData_->PushBackData(data);
		}
	}

	DrawManager::GetInstance()->DrawParticle(*particleData_,0, material_);
}
