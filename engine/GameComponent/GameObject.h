#pragma once
#include "ViewProjection.h"
#include "Material.h"
#include "WorldTransform.h"
#include "DirectionalLights.h"
#include "AnimationManager.h"
#include "ModelManager.h"
#include "SkinCluster.h"
#include <string>
#include "WaveIndexData.h"
class GameObject
{
public:
	static ModelManager* modelManager;
	static AnimationManager* animationManager;


	void Initialize(const std::string name);
	void Initialize(uint32_t modelHandle);
	void Initialize();
	void UpdateMatrix();
	void UpdateMaterial(Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	void Draw(Vector4 color = { 1.0f,1.0f,1.0f,1.0f });

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
	WorldTransform* GetWorldTransform() {
		return &worldTransform_;
	}
	uint32_t GetModelHandle() const {
		return modelHandle_;
	}
	void SetModelHandle(const uint32_t modelHandle) {
		modelHandle_ = modelHandle;
	}
protected:
	uint32_t modelHandle_;
	WorldTransform worldTransform_;
	Material material_;
};