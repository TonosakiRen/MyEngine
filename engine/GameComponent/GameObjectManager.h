#pragma once
#include <string>
#include <vector>

#include "Mymath.h"
#include "GameComponent/GameObject.h"

class GameObjectManager
{
public:
	
	static GameObjectManager* GetInstance();
	void Load();
	void Finalize();

public:

	std::vector<std::unique_ptr<GameObject>> gameObjects_;

private:
	GameObjectManager() = default;
	~GameObjectManager() = default;
	GameObjectManager(const GameObjectManager&) = delete;
	GameObjectManager& operator=(const GameObjectManager&) = delete;

};

