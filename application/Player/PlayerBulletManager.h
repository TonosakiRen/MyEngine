#pragma once
/**
 * @file PlayerBulletManager.h
 * @brief プレイヤーの弾管理
 */
#include <list>
#include <memory>
#include <vector>
#include "Mymath.h"
#include "Player/PlayerBullet.h"
#include "Particle/ExplodeParticle.h"
class PlayerBulletManager
{
public:
	void Initialize(ExplodeParticle* explodeParticle);
	void Update();
	void Draw();

	//弾発射
	void PopPlayerBullet(Vector3 position, Vector3 direction);
	//Getter
	std::list<std::unique_ptr<PlayerBullet>>& GetPlayerBullets() {
		return playerBullets_;
	}
private:
	ExplodeParticle* explodeParticle_;
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
	uint32_t modelHandle_;
};

