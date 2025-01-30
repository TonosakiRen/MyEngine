#pragma once
/**
 * @file MyGame.h
 * @brief Gameの更新描画を行う
 */
#include "Framework/Framework.h"
#include "Texture/TextureManager.h"
#include "Model/ModelManager.h"

class Renderer;
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
	
	Renderer* renderer = nullptr;
	BufferManager* bufferManager = nullptr;
	Engine::TextureManager* textureManager = nullptr;
	Engine::ModelManager* modelManager = nullptr;
	SceneManager* sceneManager = nullptr;
	GameObjectManager* gameObjectManager = nullptr;
	LightManager* lightManager_ = nullptr;
};

