#pragma once
/**
 * @file GameObject.h
 * @brief 描画されるGameObject
 */

#include <string>

#include "GameComponent/ViewProjection.h"
#include "GameComponent/Material.h"
#include "GameComponent/WorldTransform.h"
#include "Light/DirectionalLights.h"
#include "Animation/AnimationManager.h"
#include "Model/ModelManager.h"
#include "Animation/SkinCluster.h"
#include "Wave/WaveIndexData.h"

class GameObject
{
	friend class GameObjectManager;
public:
	static Engine::ModelManager* modelManager;
	static Engine::AnimationManager* animationManager;


	void Initialize(const std::string name);
	void Initialize(uint32_t modelHandle);
	void Initialize();
	//Matrix更新
	void UpdateMatrix();
	//マテリアル更新
	void UpdateMaterial(Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	void Draw(Vector4 color = { 1.0f,1.0f,1.0f,1.0f });

	//Getter
	WorldTransform* GetWorldTransform() {
		return &worldTransform_;
	}
	uint32_t GetModelHandle() const {
		return modelHandle_;
	}
	//Setter
	void SetEnableLighting(bool enableLighting) {
		material_.enableLighting_ = enableLighting;
	}
	void SetParent(WorldTransform* parent) {
		worldTransform_.SetParent(parent);
	}
	void SetScale(Vector3 scale) {
		worldTransform_.scale_ = scale;
	}
	void SetPosition(Vector3 position) {
		worldTransform_.translation_ = position;
	}
	void SetModelHandle(const uint32_t modelHandle) {
		modelHandle_ = modelHandle;
	}
protected:
	bool disable_ = false;
	uint32_t modelHandle_;
	WorldTransform worldTransform_;
	Material material_;
};