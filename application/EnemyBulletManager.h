#pragma once
#include <list>
#include <memory>
#include <vector>
#include "Mymath.h"
#include "EnemyBullet.h"
class EnemyBulletManager
{
public:
	void Initialize();
	void Update();
	void PopEnemyBullet(Vector3 position,Vector3 direction);
	void Draw();

	std::list<std::unique_ptr<EnemyBullet>>& GetEnemyBullets() {
		return enemyBullets_;
	}
private:
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	uint32_t modelHandle_;
};

