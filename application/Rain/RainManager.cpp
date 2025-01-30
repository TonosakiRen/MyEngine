/**
 * @file RainManager.h
 * @brief 雨の管理
 */
#include "Enemy/Enemy.h"
#include "RainManager.h"
#include "Stage/Floor.h"
#include "Draw/DrawManager.h"

void RainManager::Initialize(ExplodeParticle* explodeParticle)
{
	explodeParticle_ = explodeParticle;
	pointLights_ = LightManager::GetInstance()->pointLights_.get();
	particle_ = std::make_unique<ParticleModelData>(rainDropNum_);
	particle_->Initialize();
	material_ = std::make_unique<Material>();
	material_->Initialize();
	material_->enableLighting_ = false;
	material_->Update();
}

void RainManager::Update()
{
	spawnFrame_++;
	const uint32_t spawnInterval = 15;

	//出現
	if (spawnFrame_ >= spawnInterval) {
		spawnFrame_ = 0;
		PopRainDrop();
	}

	for (int i = 0; i < rainDropNum_; i++) {
		if (rainDrops_[i].isActive_) {
			rainDrops_[i].Update();
		}
	}


}

void RainManager::PopRainDrop()
{
	//isActiveがfalseのものに割り当て
	uint32_t modelHandle = Engine::ModelManager::Load("dodecahedron.obj");
	Vector4 color  = HSVAtoRGBA(Rand(0.0f, 1.0f), 1.0f, 1.0f, 1.0f);
	for (int i = 0; i < PointLights::lightNum; i++) {
		if (pointLights_->lights_[i].isActive == false) {
			for (int j = 0; j < rainDropNum_; j++) {
				if (!rainDrops_[j].isActive_) {
					rainDrops_[j].Initialize(modelHandle, &pointLights_->lights_[i],explodeParticle_, {Rand(-Floor::kFloorHalfSize,Floor::kFloorHalfSize),20.0f,Rand(-Floor::kFloorHalfSize,Floor::kFloorHalfSize)}, color);
					break;
				}
			}
			break;
		}
	}
}


void RainManager::Draw()
{

	for (size_t i = 0; i < rainDropNum_; i++)
	{
		if (rainDrops_[i].isActive_) {
			ParticleModelData::Data data;
			data.matWorld = rainDrops_[i].worldTransform_.matWorld_;
			data.worldInverseTranspose = Inverse(rainDrops_[i].worldTransform_.matWorld_);
			data.color = rainDrops_[i].color_;
			particle_->PushBackData(data);
		}
	}

	Engine::DrawManager::GetInstance()->DrawParticleModel(*particle_, Engine::ModelManager::GetInstance()->Load("dodecahedron.obj"), *material_.get());
}
