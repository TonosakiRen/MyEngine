#pragma once
#include <memory>
#include <list>
#include "BaseScene.h"
#include "MyMath.h"

#include  "Skydome.h"
#include  "BoxArea.h"
#include  "Floor.h"
#include  "Player.h"
#include  "GameObject.h"
#include  "PlayerBulletManager.h"
#include  "ExplodeParticle.h"
#include  "WhiteParticle.h"
#include  "Enemy.h"
#include  "EnemyBulletManager.h"


class GamePlayScene :
    public BaseScene
{
public:
    void Initialize() override;
    void Finalize() override;
    void Update() override;

    void ModelDraw() override;

	void ShadowDraw() override;
	void SpotLightShadowDraw() override;

	void ParticleDraw() override;
	void ParticleBoxDraw() override;

	void PreSpriteDraw() override;
	void PostSpriteDraw() override;

	static const Player* player;

private:
	void CheckAllCollision();
	void EnemySpawn(const Vector3& position);

private:
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<BoxArea> boxArea_;
	std::unique_ptr<Floor> floor_;
	std::unique_ptr<GameObject> sphere_;

	std::unique_ptr<Player> player_;
	//PlayerBullets
	std::unique_ptr<PlayerBulletManager> playerBulletManager_;

	std::unique_ptr <ExplodeParticle> explodeParticle_;

	std::unique_ptr<WhiteParticle> whiteParticle_;
	

	//Enemy
	std::list<std::unique_ptr<Enemy>> enemies_;
	//EnemyBullets
	std::unique_ptr<EnemyBulletManager> enemyBulletManager_;
	//Enemyの数
	uint32_t enemyNum_ = 0;
	//Enemyが出現するフレーム
	uint32_t enemySpawnFrame_ = 0;

};

