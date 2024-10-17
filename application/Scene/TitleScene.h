#pragma once
#include "Scene/BaseScene.h"
#include <memory>

#include "Texture/SpriteData.h"
class Input;
class GameObject;
class TitleScene :
    public BaseScene
{
	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;
private:
	Input* input_ = nullptr;
	std::unique_ptr<GameObject> title_;
	float titleT_;
	bool isTitleUp_ = false;
	std::unique_ptr<SpriteData> pushSpace_;
};

