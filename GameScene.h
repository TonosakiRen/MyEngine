#pragma once
#include "DirectXCommon.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Input.h"
#include "Sprite.h"
#include "DirectionalLight.h"
#include "Particle.h"
#include "GameObject.h"
#include "Skydome.h"
#include "Floor.h"
#include "Player.h"
#include "Boss.h"
#include "FollowCamera.h"
#include "Ground.h"
#include "GoalBox.h"
class GameScene
{

public:
	GameScene();
	~GameScene();

	void Initialize();
	void Update();
	void ModelDraw();
	void ParticleDraw();
	void ParticleBoxDraw();
	void PreSpriteDraw();
	void PostSpriteDraw();
	void Draw();


private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;

	ViewProjection viewProjection_;
	FollowCamera followCamera_;

	DirectionalLight directionalLight_;
	float cameraT_ = 0.0f;

	uint32_t textureHandle_;

	std::unique_ptr<Sprite>sprite_;
	WorldTransform spriteTransform_;

	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Floor> floor_;
	std::unique_ptr<GameObject> sphere_;
	std::unique_ptr<Player> player_;
	/*std::unique_ptr<Boss> boss_;*/

	std::list<std::unique_ptr<Boss>> bosses_;

	std::unique_ptr<Particle> particle_;

	std::unique_ptr<Ground> ground_;
	std::unique_ptr<Ground> bossGround_;
	std::unique_ptr<Ground> goalGround_;
	uint32_t blockHandle_;

	std::unique_ptr<GoalBox> goalBox_;

};
