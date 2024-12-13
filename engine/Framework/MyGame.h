#pragma once
/**
 * @file MyGame.h
 * @brief Gameの更新描画を行う
 */
#include "Framework/Framework.h"

class Renderer;
class GameScene;
class TextureManager;
class ModelManager;
class SceneManager;
class GameObjectManager;
class LightManager;
class BufferManager;


class MyGame : public Framework
{
public:
	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;
public:
	
	GameScene* gameScene = nullptr;
	Renderer* renderer = nullptr;
	BufferManager* bufferManager = nullptr;
	TextureManager* textureManager = nullptr;
	ModelManager* modelManager = nullptr;
	SceneManager* sceneManager = nullptr;
	GameObjectManager* gameObjectManager = nullptr;
	LightManager* lightManager_ = nullptr;
};

