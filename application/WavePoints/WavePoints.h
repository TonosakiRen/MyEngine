#pragma once
/**
 * @file WavePoints.h
 * @brief 波の発生ポイント達
 */
#include <memory>
#include "Wave/WaveData.h"
#include "GameComponent/WorldTransform.h"
#include "GameComponent/Material.h"
#include "Particle/ParticleModelData.h"
class WavePoints
{
public:
	static const uint32_t kWavePointNum = 256;

	struct Point {
		Vector3 position_ = { 0.0f,3.0f,0.0f };
		float t_ = 0.0f;
	};

	WavePoints();
	void Initialize();
	void Update();
	void Draw();

	//point出現
	uint32_t EmitPoint(const Vector3& pos);
	//Getter
	const WaveData* GetWaveData() const { return waveData_.get(); }
	const Point* GetPoints() const { return points; }
private:
	Point points[kWavePointNum];
	std::unique_ptr<WaveData> waveData_;
	std::unique_ptr<ParticleModelData> particle_;
};

