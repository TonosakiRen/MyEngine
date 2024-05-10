#pragma once
#include "ViewProjection.h"
#include "Material.h"
#include "WorldTransform.h"
#include "DirectionalLights.h"
#include "AnimationManager.h"
#include "ModelManager.h"
#include "SkinCluster.h"
#include <string>
class GameObject
{
public:
	static ModelManager* modelManager;
	static AnimationManager* animationManager;


	void Initialize(const std::string name);
	void Initialize(uint32_t modelHandle);
	void UpdateMatrix();
	void UpdateMaterial(Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	void Draw(uint32_t textureHandle ,Vector4 color = { 1.0f,1.0f,1.0f,1.0f } );
	void Draw(Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	void Draw(const WorldTransform& worldTransform, uint32_t textureHandle, Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	void Draw(const WorldTransform& worldTransform, Vector4 color = { 1.0f,1.0f,1.0f,1.0f });

	void SkyDraw();

	void SkinningDraw(const SkinCluster& skinCluster);

	void Draw(uint32_t modelHandle, const WorldTransform& worldTransform);

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
protected:
	uint32_t modelHandle_;
	WorldTransform worldTransform_;
	Material material_;
};