#pragma once
/**
 * @file GameScene.h
 * @brief Sceneを走らせているクラス
 */
#include "Graphics/DirectXCommon.h"
#include "Model/Model.h"
#include "Sky/Sky.h"
#include "GameComponent/ViewProjection.h"
#include "GameComponent/DebugCamera.h"
#include "Camera/Camera.h"
#include "GameComponent/WorldTransform.h"
#include "Input.h"
#include "Audio.h"
#include "Texture/Sprite.h"
#include "GameComponent/GameObject.h"
#include "Scene/SceneManager.h"
#include "Light/LightManager.h"
#include "WavePoints/WavePoints.h"

#include <optional>
class GameScene
{

public:
	GameScene();
	~GameScene();

	void Initialize();
	void Update();
	void Draw();

	//Getter
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
};

