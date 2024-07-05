#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "CommandContext.h"
#include "Material.h"
#include "WorldTransform.h"
#include "SkinCluster.h"
#include "ParticleModelData.h"
#include "ParticleData.h"
#include "SpriteData.h"

#include "Model.h"
#include "MeshletModel.h"
#include "Skinning.h"
#include "Particle.h"
#include "ParticleModel.h"
#include "Sprite.h"
#include "ShadowMap.h"
#include "SpotLightShadowMap.h"
#include "Sky.h"
#include "FloorRenderer.h"
#include "ViewProjection.h"

#include "LightManager.h"
#include "Calling.h"

class DrawManager
{
friend class Renderer;
public:

	static const uint32_t kMaxDrawCall = 4096;
	uint32_t drawCallNum_ = 0;

	enum Call {
		kModel,
		kMeshletModel,
		kParticle,
		kParticleModel,
		kPreSprite,
		kPostSprite,
		kShadow,
		kSpotLightShadow,
		kSky,
		kFloor,

		kSkinning,

		kCallNum,
	};

	static DrawManager* GetInstance();

	void DrawModel(const WorldTransform& worldTransform, const uint32_t modelHandle = 0,const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
	void DrawModel(const WorldTransform& worldTransform, const uint32_t modelHandle, SkinCluster& skinCluster, const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
	void DrawMeshletModel(const WorldTransform& worldTransform, const uint32_t modelHandle = 0, const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
	void DrawMeshletModel(const WorldTransform& worldTransform, const uint32_t modelHandle, SkinCluster& skinCluster, const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
	void DrawParticle(ParticleData& bufferData, const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
	void DrawParticleModel(ParticleModelData& bufferData, const uint32_t modelHandle = 0, const Material& material = *defaultMaterial_.get());
	void DrawPreSprite(SpriteData& spriteData);
	void DrawPostSprite(SpriteData& spriteData);
	void DrawShadow(const WorldTransform& worldTransform, const uint32_t modelHandle = 0);
	void DrawSpotLightShadow(const WorldTransform& worldTransform, const uint32_t modelHandle = 0);
	void DrawSky(const WorldTransform& worldTransform);
	void DrawFloor(const WorldTransform& worldTransform, const uint32_t modelHandle = 0);

	void SetCallingViewProjection(const ViewProjection& viewProjection) { calling_->SetViewProjection(&viewProjection); }
private:

	void Initialize(CommandContext& CommandContext);
	void Finalize();
	void AllDraw(const ViewProjection& viewProjection);
	void ShadowDraw();
	void ShadowSpotLightDraw();
	void ResetCalls();

private:
	std::unique_ptr<Model> modelPipeline_;
	std::unique_ptr<MeshletModel> meshletModelPipeline_;
	std::unique_ptr<Skinning> skinningPipeline_;
	std::unique_ptr<Particle> particlePipeline_;
	std::unique_ptr<ParticleModel> particleModelPipeline_;
	std::unique_ptr<Sprite> spritePipeline_;
	std::unique_ptr<ShadowMap> shadowPipeline_;
	std::unique_ptr<SpotLightShadowMap> spotLightShadowPipeline_;
	std::unique_ptr<Sky> skyPipeline_;
	std::unique_ptr<FloorRenderer> floorPipeline_;

	static std::unique_ptr<Material> defaultMaterial_;

	std::unique_ptr<Calling> calling_;
	CommandContext* commandContext_;

	LightManager* lightManager_;

	std::vector<std::function<void()>> calls[kCallNum];
private:
	DrawManager() = default;
	~DrawManager() = default;
	DrawManager(const DrawManager&) = delete;
	DrawManager& operator=(const DrawManager&) = delete;
};

