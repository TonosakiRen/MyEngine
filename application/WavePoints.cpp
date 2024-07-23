#include "WavePoints.h"
#include "ImGuiManager.h"
#include "DrawManager.h"

WavePoints::WavePoints()
{
	waveData_ = std::make_unique<WaveData>(kWavePointNum);
	particle_ = std::make_unique<ParticleModelData>(kWavePointNum);
}

void WavePoints::Initialize()
{
	waveData_->Initialize();
	particle_->Initialize();
}

void WavePoints::Update() {
	ImGui::DragFloat3("wavePosition", &points[0].position_.x, 0.1f);
	ImGui::DragFloat("waveT", &points[0].t_, 0.01f,0.1f,1.0f);
	waveData_->Copy(points);
}

void WavePoints::Draw()
{

	for (size_t i = 0; i < kWavePointNum; i++)
	{
		if (points[i].t_ > 0.0f) {
			ParticleModelData::Data data;
			data.matWorld = MakeAffineMatrix({ 0.1f,0.1f,0.1f }, {0.0f,0.0f,0.0f,1.0f}, points[i].position_);
			data.worldInverseTranspose = Inverse(data.matWorld);
			particle_->PushBackData(data);
		}
	}

	DrawManager::GetInstance()->DrawParticleModel(*particle_, ModelManager::GetInstance()->Load("sphere.obj"));
}
