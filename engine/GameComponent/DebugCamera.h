#pragma once
/**
 * @file DebugCamera.h
 * @brief デバッグ用カメラ
 */
#include <memory>

#include "GameComponent/ViewProjection.h"
class DebugCamera :
	public ViewProjection
{
public:
	void Initialize();
	void Initialize(Vector3 position,Vector3 rotation);
	void Update();
private:
	//デバック用マウス入力受けつけ
	void DebugMode();
	Vector3 debugRotation_ = { 0.0f,0.0f,0.0f };
};

