#pragma once
#include "MyMath.h"
#include "GameComponent/Collider.h"
#include "GameComponent/GameObject.h"

class EnemyBulletManager;
class Player;
class LightManager;

class LineAttack :
	public GameObject
{
public:

	struct Box{
		WorldTransform worldTransform_;
		Vector3 direction_;
		uint32_t attackStartFrame_;
		uint32_t endFrame_;
		bool isActive_;
	};

	void Initialize();
	void Update();
	void Draw();

	void Emit() { isEmit_ = true; };
	bool GetIsEmit() { return isEmit_; }
private:
	LightManager* lightManager_ = nullptr;
	std::vector<std::unique_ptr<Box>> boxs_;
	bool isEmit_ = false;
	bool isBox_ = false;
	float lightLength_ = 0.0f;
	const float kEmitLength_ = 1.0f;
	float emitLength_ = kEmitLength_;
	float lastLength_ = 0.0f;
	float scale_ = 6.0f;
};

