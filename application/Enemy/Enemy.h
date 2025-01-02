#pragma once
/**
 * @file Enemy.h
 * @brief 敵
 */
#include "MyMath.h"
#include "GameComponent/Collider.h"
#include "GameComponent/GameObject.h"

class EnemyBulletManager;
class Player;

class Enemy :
	public GameObject
{
public:
	static uint32_t deadEnemyNum;
	static Vector3 modelSize;

	void Initialize(uint32_t modelHandle, Vector3 position, EnemyBulletManager* enemyBulletManager,Player* player, Vector3 direction);
	void Update();
	void OnCollision();
	void Draw();

	//Getter
	bool IsDead() const { return isDead_; }
	Collider& GetCollider() { return collider_; }
private:
	/// <summary>
	/// 攻撃
	/// </summary>
	void Fire();
private:
	//enemyBulletManager
	EnemyBulletManager* enemyBulletManager_;
	//Player
	Player* player_;
	//collider
	Collider collider_;
	//速度
	float speed_ = 0.06f;
	// デスフラグ
	bool isDead_ = false;
	//Fireのフレーム
	uint32_t fireFrame_ = 0;
	//方向
	Vector3 direction_;
};

