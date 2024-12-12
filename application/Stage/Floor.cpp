#include "Stage/Floor.h"
#include "Texture/TextureManager.h"
#include "ImGuiManager.h"
#include "Draw/DrawManager.h"
#include "Scene/GameScene.h"

const float Floor::kFloorSize = 50.0f;
const float  Floor::kFloorHalfSize = Floor::kFloorSize / 2.0f;

void Floor::Initialize(const std::string name,WavePoints* wavePoints)
{
	GameObject::Initialize(name);
	material_.enableLighting_ = true;
	material_.Update();
	worldTransform_.scale_ = { kFloorSize, kFloorSize, kFloorSize };
	worldTransform_.Update();
	textureHandle_ = TextureManager::Load("floorTile.png");

	waveIndexData_.Initialize();
	wavePoints_ = wavePoints;
}

void Floor::Update()
{
	worldTransform_.Update();
	const WavePoints::Point* points = wavePoints_->GetPoints();
	for (auto it = waveIndexData_.index_.begin(); it != waveIndexData_.index_.end();) {
		if (points[*it].t_ <= 0) {
			it = waveIndexData_.index_.erase(it);
		}
		else {
			++it;
		}
	}
	waveIndexData_.Update();
}

void Floor::Draw()
{
	DrawManager::GetInstance()->DrawFloor(worldTransform_, *GameScene::wavePoints->GetWaveData(), waveIndexData_, modelHandle_);

}

