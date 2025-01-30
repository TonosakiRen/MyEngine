#pragma once
/**
 * @file DrawManager.h
 * @brief DrawCallをまとめる
 */
#include <vector>
#include <memory>
#include <functional>

#include "Graphics/CommandContext.h"
#include "GameComponent/Material.h"
#include "GameComponent/WorldTransform.h"
#include "Animation/SkinCluster.h"
#include "Particle/ParticleModelData.h"
#include "Particle/ParticleData.h"
#include "Texture/SpriteData.h"
#include "Model/Model.h"
#include "Mesh/MeshletModel.h"
#include "Mesh/GlitchMeshletModel.h"
#include "Mesh/MeshletEnvironmentMapModel.h"
#include "Animation/Skinning.h"
#include "Particle/Particle.h"
#include "Particle/ParticleModel.h"
#include "Texture/Sprite.h"
#include "Light/ShadowMap.h"
#include "Light/SpotLightShadowMap.h"
#include "Sky/Sky.h"
#include "Wave/FloorRenderer.h"
#include "Wave/WaveModel.h"
#include "GameComponent/ViewProjection.h"
#include "Light/TileBasedRendering.h"
#include "Light/LightManager.h"
#include "Draw/Calling.h"
#include "Raytracing/Raytracing.h"
#include "Curve/LissajousCurve.h"

namespace Engine {

	class DrawManager
	{
		friend class Renderer;
	public:

		static const uint32_t kMaxDrawCall = 4096;
		uint32_t drawCallNum_ = 0;

		enum Call {
			kModel,
			kMeshletModel,
			kGlitchMeshletModel,
			kMeshletEnvironmentMap,
			kParticle,
			kParticleModel,
			kPostParticleModel,
			kPreSprite,
			kPostSprite,
			kShadow,
			kSpotLightShadow,
			kSky,
			kFloor,
			kWave,

			kSkinning,

			kCallNum,
		};

		static DrawManager* GetInstance();

		//各パイプラインDrawCall
		void DrawModel(const WorldTransform& worldTransform, const uint32_t modelHandle = 0, const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
		void DrawModel(const WorldTransform& worldTransform, const uint32_t modelHandle, SkinCluster& skinCluster, const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
		void DrawMeshletModel(const WorldTransform& worldTransform, const uint32_t modelHandle = 0, const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
		void DrawMeshletModel(const WorldTransform& worldTransform, const uint32_t modelHandle, SkinCluster& skinCluster, const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
		void DrawGlitchMeshletModel(const WorldTransform& worldTransform, const uint32_t modelHandle = 0, const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
		void DrawEnvironmentMapMeshletModel(const WorldTransform& worldTransform, const uint32_t modelHandle = 0, const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
		void DrawEnvironmentMapMeshletModel(const WorldTransform& worldTransform, const uint32_t modelHandle, SkinCluster& skinCluster, const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
		void DrawParticle(ParticleData& bufferData, const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
		void DrawParticleModel(ParticleModelData& bufferData, const uint32_t modelHandle = 0, const Material& material = *defaultMaterial_.get());
		void DrawPostParticleModel(ParticleModelData& bufferData, const uint32_t modelHandle = 0, const Material& material = *defaultMaterial_.get());
		void DrawPreSprite(SpriteData& spriteData);
		void DrawPostSprite(SpriteData& spriteData);
		void DrawShadow(const WorldTransform& worldTransform, const uint32_t modelHandle = 0);
		void DrawSpotLightShadow(const WorldTransform& worldTransform, const uint32_t modelHandle = 0);
		void DrawSky(const WorldTransform& worldTransform);
		void DrawWaveModel(const WorldTransform& worldTransform, const WaveData& waveData, const WaveIndexData& waveIndexData, const uint32_t modelHandle = 0, const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
		void DrawWaveModel(const WorldTransform& worldTransform, const WaveData& waveData, const WaveIndexData& waveIndexData, const uint32_t modelHandle, SkinCluster& skinCluster, const uint32_t textureHandle = 0, const Material& material = *defaultMaterial_.get());
		void DrawFloor(const WorldTransform& worldTransform, const WaveData& waveData, const WaveIndexData& waveIndexData, const uint32_t modelHandle = 0);

		//カリングするViewProjectionのセッター
		void SetCallingViewProjection(const ViewProjection& viewProjection) { calling_->SetViewProjection(&viewProjection); }
	private:

		void Initialize(CommandContext& CommandContext);
		void Finalize();

		//すべてのDrawCall実行
		void AllDraw(PipelineType pipelineType, const ViewProjection& viewProjection, const TileBasedRendering& tileBasedRendering);
		//背景画像DrawCall実行
		void PostSpriteDraw();
		//影DrawCall
		void ShadowDraw();
		//SpotLight影DrawCall
		void ShadowSpotLightDraw();

		//DrawCallReset
		void ResetCalls();

	private:
		std::unique_ptr<Model> modelPipeline_;
		std::unique_ptr<MeshletModel> meshletModelPipeline_;
		std::unique_ptr<GlitchMeshletModel> glitchMeshletModelPipeline_;
		std::unique_ptr<MeshletEnvironmentMapModel> meshletEnvironmentMapPipeline_;
		std::unique_ptr<Skinning> skinningPipeline_;
		std::unique_ptr<Particle> particlePipeline_;
		std::unique_ptr<ParticleModel> particleModelPipeline_;
		std::unique_ptr<Sprite> spritePipeline_;
		std::unique_ptr<ShadowMap> shadowPipeline_;
		std::unique_ptr<SpotLightShadowMap> spotLightShadowPipeline_;
		std::unique_ptr<Sky> skyPipeline_;
		std::unique_ptr<FloorRenderer> floorPipeline_;
		std::unique_ptr<WaveModel> waveModelPipeline_;

		std::unique_ptr<Raytracing> raytracing_;

		static std::unique_ptr<Material> defaultMaterial_;
		std::unique_ptr<LissajousCurve> lissajousCurve_;

		std::unique_ptr<Calling> calling_;
		CommandContext* commandContext_;

		LightManager* lightManager_;
		ModelManager* modelManager_;

		std::vector<std::function<void()>> calls[kCallNum];
	private:
		DrawManager() = default;
		~DrawManager() = default;
		DrawManager(const DrawManager&) = delete;
		DrawManager& operator=(const DrawManager&) = delete;
	};

}