#pragma once
#include "DirectXCommon.h"
#include "Model.h"
#include "ViewProjection.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "Input.h"
#include "Audio.h"
#include "Sprite.h"
#include "DirectionalLights.h"
#include "PointLights.h"
#include "SpotLights.h"
#include "ShadowSpotLights.h"
#include "Compute.h"
#include "GameObject.h"
#include "SceneManager.h"


#include <optional>
class GameScene
{

public:
	GameScene();
	~GameScene();

	void Initialize();
	void Update(CommandContext& commandContext);
	void Draw(CommandContext& commandContext);
	void ShadowMapDraw(CommandContext& commandContext);
	void SpotLightShadowMapDraw(CommandContext& commandContext);
	void UIDraw(CommandContext& commandContext);

	DirectionalLights& GetDirectionalLights() {
		return *directionalLights_.get();
	}

	PointLights& GetPointLights() {
		return *pointLights_.get();
	}

	SpotLights& GetSpotLights() {
		return *spotLights_.get();
	}

	ShadowSpotLights& GetShadowSpotLights() {
		return *shadowSpotLights_.get();
	}

	const ViewProjection& GetViewProjection() {
		return *currentViewProjection_;
	}

public:
	static ViewProjection* currentViewProjection_;
	static DirectionalLights* directionLights;
	static PointLights* pointLights;
	static SpotLights* spotLights;
	static ShadowSpotLights* shadowSpotLights;

private: 
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	SceneManager* sceneManager_ = nullptr;

	std::unique_ptr <DebugCamera> debugCamera_;
	
	std::unique_ptr<Camera> camera_;
	std::unique_ptr <DirectionalLights> directionalLights_;
	std::unique_ptr <PointLights> pointLights_;
	std::unique_ptr <SpotLights> spotLights_;
	std::unique_ptr <ShadowSpotLights> shadowSpotLights_;

	std::unique_ptr<Compute> compute_;

};

