#pragma once
/**
 * @file BaseScene.h
 * @brief sceneの仮想Class
 */
#include "WavePoints/WavePoints.h"
#include "GameComponent/DebugCamera.h"
#include "GameComponent/ViewProjection.h"
#include "Camera/Camera.h"
#include "Light/LightManager.h"
#include "Player/Player.h"
class SceneManager;
class BaseScene
{
public:

	static ViewProjection* currentViewProjection;
	static WavePoints* wavePoints;

	virtual ~BaseScene() = default;

	//ゲームの最初だけいる処理
	static void StaticInitialize();
	//シーンにおける共通処理
	static void CommonUpdate();
	//ゲームの最後だけいる処理
	static void StaticFinalize();

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	//setter
	virtual void SetSceneManager(SceneManager* sceneManager) {
		sceneManager_ = sceneManager;
	}
protected:
	SceneManager* sceneManager_ = nullptr;
	static std::unique_ptr<Player> player_;
	static std::unique_ptr <DebugCamera> debugCamera_;
	static std::unique_ptr<WavePoints> wavePoints_;
	static std::unique_ptr<Camera> camera_;
};

