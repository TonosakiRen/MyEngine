#pragma once
/**
 * @file GamePlayScene.h
 * @brief gamePlayのSceneクラス
 */
#include <memory>
#include <list>
#include "Scene/BaseScene.h"
#include "MyMath.h"
#include "Input.h"

#include  "Stage/Skybox.h"
#include  "Stage/Floor.h"
#include  "Stage/Cave.h"
#include  "Player/Player.h"
#include  "GameComponent/GameObject.h"
#include  "Player/PlayerBulletManager.h"
#include  "Particle/ExplodeParticle.h"
#include  "Enemy/Enemy.h"
#include  "Enemy/EnemyBulletManager.h"
#include  "Particle/SphereLights.h"
#include "GameComponent/GameObjectManager.h"
#include "Stage/Trees.h"
#include "Enemy/LineAttack.h"
#include "Stage/LightObjects.h"
#include "Rain/RainManager.h"


class GamePlayScene :
    public BaseScene
{
public:
    void Initialize() override;
    void Finalize() override;
    void Update() override;

    void Draw() override;

private:
	//すべての当たり判定
	void CheckAllCollision();

private:
	Engine::Input* input_;

	std::unique_ptr<Skybox> skybox_;
	std::unique_ptr<Floor> floor_;
	std::unique_ptr<GameObject> sphere_;
	std::unique_ptr < Trees> trees_;
	//PlayerBullets
	std::unique_ptr<PlayerBulletManager> playerBulletManager_;

	std::unique_ptr <ExplodeParticle> explodeParticle_;

	std::unique_ptr<LineAttack> lineAttack_;
	
	std::unique_ptr<RainManager> rainManager_;

	std::unique_ptr<Cave> cave_;

	std::vector<std::unique_ptr<GameObject>>* gameObjects_;
};

