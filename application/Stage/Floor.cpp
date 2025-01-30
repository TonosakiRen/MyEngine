/**
 * @file Floor.cpp
 * @brief 床
 */

#include "Stage/Floor.h"
#include "Texture/TextureManager.h"
#include "ImGuiManager.h"
#include "Draw/DrawManager.h"
#include "Scene/BaseScene.h"

const float Floor::kFloorSize = 50.0f;
const float  Floor::kFloorHalfSize = Floor::kFloorSize / 2.0f;

void Floor::Initialize(const std::string name,WavePoints* wavePoints)
{
	GameObject::Initialize(name);
	material_.enableLighting_ = true;
	material_.Update();
	worldTransform_.scale_ = { kFloorSize, kFloorSize, kFloorSize };
	worldTransform_.Update();
	textureHandle_ = Engine::TextureManager::Load("floorTile.png");

	waveIndexData_.Initialize();
	wavePoints_ = wavePoints;
}

void Floor::Update()
{
	worldTransform_.Update();
	const WavePoints::Point* points = wavePoints_->GetPoints();
	//波のtが0.0f以下のものをindexから削除
	for (auto it = waveIndexData_.GetIndex().begin(); it != waveIndexData_.GetIndex().end();) {
		if (points[*it].t_ <= 0) {
			it = waveIndexData_.GetIndex().erase(it);
		}
		else {
			++it;
		}
	}
	waveIndexData_.Update();
}

void Floor::Draw()
{
	Engine::DrawManager::GetInstance()->DrawFloor(worldTransform_, *BaseScene::wavePoints->GetWaveData(), waveIndexData_, modelHandle_);

}

