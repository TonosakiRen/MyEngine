#pragma once
/**
 * @file Camera.h
 * @brief カメラ構造体
 */
#include "GameComponent/ViewProjection.h"
#include <memory>
class Camera :
	public ViewProjection
{
public:
	void Initialize();
	void Initialize(Vector3 position, Quaternion rotation);
	void Update(Vector3 playerPos);
	void Update();
	//Getter
	Vector3 GetOffset();
private:
	Vector3 savePlayerPos;
	Vector3 interTarget_;
	Vector3 rotate_;
};