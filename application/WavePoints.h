#pragma once
#include <memory>
#include "WaveData.h"
#include "WorldTransform.h"
#include "Material.h"
#include "ParticleModelData.h"
class WavePoints
{
public:
	static const uint32_t kWavePointNum = 50;
	WavePoints();
	void Initialize();
	void Update();
	void Draw();

	const WaveData* GetWaveData() const { return waveData_.get(); }
	
	struct Point {
		Vector3 position_ = {0.0f,3.0f,0.0f};
		float t_ = 0.0f;
	};
private:
	Point points[kWavePointNum];
	std::unique_ptr<WaveData> waveData_;
	std::unique_ptr<ParticleModelData> particle_;
};

