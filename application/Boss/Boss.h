#pragma once
#include <memory>
#include <functional>

#include "GameComponent/GameObject.h"
#include "GameComponent/Collider.h"
#include "Mymath.h"
#include "Animation/AnimationManager.h"
#include "Model/ModelManager.h"
class Player;
class Boss :
    public GameObject
{
public:

	static const int kBulletNum = 200;

    enum class Parts { Body, Right, Left, PartsNum };

	enum Stats {
		kNormal,
		kAttack,
		kAssault,
		kBulletAttack,

		kNumStats
	};

    void Initialize(const std::string name);
    void Update();
    void Draw();

	void SetPlayer(const Player& player) { player_ = &player; }

    const WorldTransform& GetWorldTransform() const {
        return worldTransform_;
    }
    struct Bullet {
        bool isActive_;
        WorldTransform worldTransform_;
        Vector3 direction_;
        int lifeSpan_ = 0;
    };
public:
    Collider collider_;
private:

	void Normal();
	void Attack();
	void Assault();
	void BulletAttack();
	void Random();

private:

    WorldTransform modelWorldTransform_[static_cast<int>(Boss::Parts::PartsNum)];

	std::function<void()> stateFunctions_[kNumStats];

	// 浮遊ギミックの媒介変数
	float moveRotate_ = 0.0f;
	// プレイヤー
	const Player* player_ = nullptr;
	// enemy
	Stats stats_ = kNormal;
	Stats preStats_ = kNormal;

	//行動
	bool isRandam_ = false;
	int randam_ = 0;

	// moveCoolTime
	bool isRotated_ = false;
	bool isRotate_ = false;
	Vector3 enemyToPlayer_;
	Quaternion goalRotation_;
	bool isDecidePositive_ = false;
	bool isPositive_ = false;
	int moveTime_ = 0;

	// Attack

	float attackT_ = 0.0f;
	bool isSavePos_ = false;
	Vector3 savePos_ = { 0.0f, 0.0f, 0.0f };
	const float attackSpeed_ = 0.02f;
	Vector3 goalPos_ = { 0.0f,0.0f,0.0f };
	int attackCooltime_ = 60;

	//assault
	float degree_ = 0.0f;
	float degreeSpeed_ = 0.0f;
	bool isAssault_ = false;
	int assaultTime_ = 0;
	bool isEndAssult_ = false;
	Vector3 assaultDirection_{};

	Bullet bullet_[kBulletNum];
	int bulletCooltime_ = 0;
	const float bulletSpeed_ = 0.5f;
	int bulletShotNum_ = 0;
	int bulletShotAllNum_ = 0;
	int bulletLastNum_ = 0;

	//
	bool isHit_ = false;
	int hp_ = 300;
	bool isDead_ = false;

	float length_;
};