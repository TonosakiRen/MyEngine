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
class GameScene
{

public:
	GameScene();
	~GameScene();

	void Initialize();
	void Update();
	void ModelDraw();
	void ParticleDraw();
	void PreSpriteDraw();
	void PostSpriteDraw();
	void Draw();


private: 
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	
	ViewProjection viewProjection_;
	DirectionalLight directionalLight_;

	uint32_t textureHandle_;

	std::unique_ptr<Sprite>sprite_;
	WorldTransform spriteTransform_;

	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Floor> floor_;
	std::unique_ptr<GameObject> sphere_;


	std::unique_ptr<Particle> particle_;
};
