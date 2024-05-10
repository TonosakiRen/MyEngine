#pragma once
#include "BaseScene.h"
#include <memory>
class Input;
class GameObject;
class Sprite;
class TitleScene :
    public BaseScene
{
	void Initialize() override;
	void Finalize() override;
	void Update() override;

	void ModelDraw() override;
	void SkyDraw() override;
	void SkinningDraw() override;

	void ShadowDraw() override;
	void SpotLightShadowDraw() override;

	void ParticleDraw() override;
	void ParticleBoxDraw() override;

	void PreSpriteDraw() override;
	void PostSpriteDraw() override;
private:
	Input* input_ = nullptr;
	std::unique_ptr<GameObject> title_;
	float titleT_;
	bool isTitleUp_ = false;
	std::unique_ptr<Sprite> pushSpace_;
};

