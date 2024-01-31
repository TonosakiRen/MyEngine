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

#include "Skydome.h"
#include "BoxArea.h"
#include "Floor.h"
#include "Player.h"
#include "PlayerBulletManager.h"
#include "DustParticle.h"
#include "WhiteParticle.h"
#include "Wall.h"
#include "Enemy.h"
#include "EnemyBulletManager.h"
#include "ExplodeParticle.h"


#include <optional>
class GameScene
{

public:
	GameScene();
	~GameScene();

	void Initialize();
	void Update(CommandContext& commandContext);
	void ModelDraw();
	void ShadowDraw();
	void SpotLightShadowDraw();
	void ParticleDraw();
	void ParticleBoxDraw();
	void PreSpriteDraw();
	void PostSpriteDraw();
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

	bool GetIsSceneChange() {
		return isSceneChange_;
	}

	void SetIsSceneChange(bool is) {
		isSceneChange_ = is;
	}

private: 
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;

	std::unique_ptr <DebugCamera> debugCamera_;
	ViewProjection* currentViewProjection_ = nullptr;
	
	std::unique_ptr<Camera> camera_;
	std::unique_ptr <DirectionalLights> directionalLights_;
	std::unique_ptr < PointLights> pointLights_;
	std::unique_ptr < SpotLights> spotLights_;
	std::unique_ptr < ShadowSpotLights> shadowSpotLights_;

	uint32_t textureHandle_;

	std::unique_ptr<Sprite>sprite_;
	std::unique_ptr < WorldTransform >spriteTransform_;

	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<BoxArea> boxArea_;
	std::unique_ptr<Floor> floor_;
	std::unique_ptr<GameObject> sphere_;

	std::unique_ptr<Player> player_;
	//PlayerBullets
	std::unique_ptr<PlayerBulletManager> playerBulletManager_;

	std::unique_ptr<DustParticle> dustParticle_;

	std::unique_ptr <ExplodeParticle> explodeParticle_;

	std::unique_ptr<WhiteParticle> whiteParticle_;

	std::vector<std::unique_ptr<Wall>> walls_;

	Vector4 color = {1.0f,1.0f,1.0f,1.0f};

	std::unique_ptr<Compute> compute_;

	//Enemy
	std::list<std::unique_ptr<Enemy>> enemies_;
	//EnemyBullets
	std::unique_ptr<EnemyBulletManager> enemyBulletManager_;
	//Enemyの数
	uint32_t enemyNum_ = 0;
	//Enemyが出現するフレーム
	uint32_t enemySpawnFrame_ = 0;

	GameObject title_;
	float titleT_;
	bool isTitleUp_ = false;
	Sprite pushSpace_;

	//Scene
	enum class Scene {
		Title,
		InGame,

		SceneNum
	};

	Scene scene_ = Scene::Title;
	Scene nextScene = Scene::Title;
	static void (GameScene::* SceneInitializeTable[])();
	static void (GameScene::* SceneUpdateTable[])();
	std::optional<Scene> sceneRequest_ = std::nullopt;
	bool isSceneChange_ = false;
	bool isInitialSceneChange = false;

	//タイトル
	void TitleInitialize();
	void TitleUpdate();
	//インゲーム
	void InGameInitialize();
	void InGameUpdate();
	void CheckAllCollision();
	void EnemySpawn(Vector3 position);
};

