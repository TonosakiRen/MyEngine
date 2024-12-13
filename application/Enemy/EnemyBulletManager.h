#pragma once
/**
 * @file EnemyBulletManager.h
 * @brief 敵弾の管理
 */
#include <list>
#include <memory>
#include "Mymath.h"
#include "Enemy/EnemyBullet.h"
#include "Particle/ExplodeParticle.h"

class EnemyBulletManager
{
public:


	void Initialize(ExplodeParticle* explodeParticle);
	void Update();
	void PopEnemyBullet(Vector3 position,Vector3 direction);
	void Draw();

	std::list<std::unique_ptr<EnemyBullet>>& GetEnemyBullets() {
		return enemyBullets_;
	}
private:
	ExplodeParticle* explodeParticle_;
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	uint32_t modelHandle_;
};

