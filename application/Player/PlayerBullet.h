#pragma once
#include "Mymath.h"
#include "WorldTransform.h"
#include "Collider.h"
#include "GameObject.h"

class PlayerBullet:
	public GameObject
{
public:
	static Vector3 modelSize;
	void Initialize(Vector3 position, Vector3 direction, uint32_t modelHandle);
	void Update();
	void OnCollision();
	void Draw();

	bool IsDead() const { return isDead_; }
	Collider& GetCollider() { return collider_; }
private:

private:
	//Collider
	Collider collider_;
	//方向
	Vector3 direction_;
	//速度
	float speed_ = 0.5f;
	// デスフラグ
	bool isDead_ = false;
};

