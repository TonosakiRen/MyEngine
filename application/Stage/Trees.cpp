#include "Stage/Trees.h"
#include "Draw/DrawManager.h"
#include "GameComponent/Loader.h"
#include "Stage/Floor.h"

void Trees::Initialize()
{
	trees_.resize(kTreeNum_);

	const float offset = Floor::kFloorHalfSize + 3.0f;
	const float space = Floor::kFloorSize / (kTreeNum_ / 4.0f);

	for (int i = 0; i < kTreeNum_;i++) {
		trees_[i].Initialize();
		int quot = i / 4;
		int remainder = i % 4;
		Vector3 position;
		switch (remainder)
		{
		case 0:
			position = { offset ,0.0f,offset - quot * space };
			break;
		case 1:
			position = { offset - quot * space ,0.0f,offset * -1.0f };
			break;
		case 2:
			position = { offset * -1.0f + quot * space ,0.0f,offset  };
			break;
		case 3:
			position = { offset * -1.0f ,0.0f,offset * -1.0f + quot * space };
			break;
		default:
			break;
		}
		trees_[i].GetWorldTransform()->translation_ = position;
	}
}

void Trees::Update()
{
	for (int i = 0; i < kTreeNum_; i++) {
		trees_[i].Update();
	}
}

void Trees::Draw()
{
	for (int i = 0; i < kTreeNum_; i++) {
		trees_[i].Draw();
	}
}
