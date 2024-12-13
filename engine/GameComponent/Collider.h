#pragma once
/**
 * @file Collider.h
 * @brief 当たり判定を行う
 */
#include <string>

#include "GameComponent/GameObject.h"

class Collider
{
public:

	static bool isDrawCollider;
	static void SwitchIsDrawCollider();

	void Initialize(WorldTransform* objectWorldTransform, const std::string name, Vector3 initialScale = { 1.0f,1.0f,1.0f }, Vector3 initialPos = { 0.0f,0.0f,0.0f });
	void Initialize(WorldTransform* objectWorldTransform, const std::string name,uint32_t modelHandle);
	//ただの四角形用
	void Initialize(const std::string name);
	//当たり判定
	bool Collision(Collider& colliderB);
	bool Collision(Collider& colliderB, Vector3& minAxis, float& minOverlap);
	bool Collision(Collider& colliderB, Vector3& pushBuckVector);
	bool SphereCollision(Vector3 position1,Vector3 size1, Vector3 position2, Vector3 size2);
	bool SphereCollision(Collider& colliderB);
	//scaleの調整
	void AdjustmentScale();
	//matrix更新
	void MatrixUpdate();
	void Draw();

	bool ObbCollision(const OBB& obb1, const OBB& obb2);
	bool ObbCollision(const OBB& obb1, const OBB& obb2, Vector3& minAxis, float& minOverlap);

public:
	WorldTransform worldTransform_;
private:
	std::string name_;

};