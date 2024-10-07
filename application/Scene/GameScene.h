#pragma once
#include "DirectXCommon.h"
#include "Model.h"
#include "Sky.h"
#include "ViewProjection.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "Input.h"
#include "Audio.h"
#include "Sprite.h"
#include "Compute.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "LightManager.h"
#include "WavePoints.h"

#include <optional>
class GameScene
{

public:
	GameScene();
	~GameScene();

	void Initialize();
	void Update(CommandContext& commandContext);
	void Draw();

	ViewProjection& GetViewProjection() {
		return *currentViewProjection;
	}

	ViewProjection& GetGameViewProjection() {
		return *camera_;
	}

public:
	static ViewProjection* currentViewProjection;
	static WavePoints* wavePoints;

private: 
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	SceneManager* sceneManager_ = nullptr;
	LightManager* lightManager_ = nullptr;

	std::unique_ptr <DebugCamera> debugCamera_;
	std::unique_ptr<WavePoints> wavePoints_;
	
	std::unique_ptr<Camera> camera_;
	std::unique_ptr<Compute> compute_;

};

