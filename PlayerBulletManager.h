#pragma once
#include <list>
#include <memory>
#include <vector>
#include "Mymath.h"
#include "PlayerBullet.h"
class PlayerBulletManager
{
public:
	void Initialize();
	void Update();
	void PopPlayerBullet(Vector3 position, Vector3 direction);
	void Draw();

	std::list<std::unique_ptr<PlayerBullet>>& GetPlayerBullets() {
		return playerBullets_;
	}
private:
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
	uint32_t modelHandle_;
};

