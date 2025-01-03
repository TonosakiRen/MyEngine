#pragma once
/**
 * @file EnemyBullet.h
 * @brief 敵弾
 */
#include "Mymath.h"
#include "GameComponent/WorldTransform.h"
#include "GameComponent/Collider.h"
#include "GameComponent/GameObject.h"

class EnemyBullet:
	public GameObject
{
public:
	static Vector3 modelSize;

	void Initialize(Vector3 position,Vector3 direction,uint32_t modelHandle);
	void Update();
	void OnCollision();
	void Draw();

	//Getter
	bool IsDead() const { return isDead_; }
	Collider& GetCollider() { return collider_; }
private:

private:
	//Collider
	Collider collider_;
	//方向
	Vector3 direction_;
	//速度
	float speed_ = 0.06f;
	// デスフラグ
	bool isDead_ = true;
};

