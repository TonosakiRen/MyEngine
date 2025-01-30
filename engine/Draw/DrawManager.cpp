/**
 * @file DrawManager.cpp
 * @brief DrawCallをまとめる
 */
#include "Draw/DrawManager.h"

#include <DirectXTex.h>
#include <cassert>

#include "Render/Renderer.h"

using namespace DirectX;

namespace Engine {

	std::unique_ptr<Material> DrawManager::defaultMaterial_;

	void DrawManager::Finalize()
	{
		modelPipeline_.reset();
		meshletModelPipeline_.reset();
		glitchMeshletModelPipeline_.reset();
		meshletEnvironmentMapPipeline_.reset();
		skinningPipeline_.reset();
		particlePipeline_.reset();
		particleModelPipeline_.reset();
		spritePipeline_.reset();
		shadowPipeline_.reset();
		spotLightShadowPipeline_.reset();
		skyPipeline_.reset();
		defaultMaterial_.reset();
		floorPipeline_.reset();
		waveModelPipeline_.reset();
		raytracing_.reset();
		lissajousCurve_.reset();

		calls->clear();
	}


	DrawManager* DrawManager::GetInstance() {
		static DrawManager instance;
		return &instance;
	}

	void DrawManager::Initialize(CommandContext& CommandContext)
	{
		lightManager_ = LightManager::GetInstance();
		modelManager_ = ModelManager::GetInstance();

		commandContext_ = &CommandContext;
		defaultMaterial_ = std::make_unique<Material>();
		defaultMaterial_->Initialize();
		defaultMaterial_->Update();
		modelPipeline_ = std::make_unique<Model>();
		modelPipeline_->Initialize();
		meshletModelPipeline_ = std::make_unique<MeshletModel>();
		meshletModelPipeline_->Initialize();
		skinningPipeline_ = std::make_unique<Skinning>();
		skinningPipeline_->Initialize();
		particlePipeline_ = std::make_unique<Particle>();
		particlePipeline_->Initialize();
		particleModelPipeline_ = std::make_unique<ParticleModel>();
		particleModelPipeline_->Initialize();
		spritePipeline_ = std::make_unique<Sprite>();
		spritePipeline_->Initialize();
		shadowPipeline_ = std::make_unique<ShadowMap>();
		shadowPipeline_->Initialize();
		spotLightShadowPipeline_ = std::make_unique<SpotLightShadowMap>();
		spotLightShadowPipeline_->Initialize();
		skyPipeline_ = std::make_unique<Sky>();
		skyPipeline_->Initialize(CommandContext);
		glitchMeshletModelPipeline_ = std::make_unique<GlitchMeshletModel>();
		glitchMeshletModelPipeline_->Initialize(CommandContext);
		meshletEnvironmentMapPipeline_ = std::make_unique<MeshletEnvironmentMapModel>();
		meshletEnvironmentMapPipeline_->Initialize(*skyPipeline_.get());
		floorPipeline_ = std::make_unique<FloorRenderer>();
		floorPipeline_->Initialize(CommandContext);
		waveModelPipeline_ = std::make_unique<WaveModel>();
		waveModelPipeline_->Initialize();
		raytracing_ = std::make_unique<Raytracing>();
		raytracing_->Initialize();

		lissajousCurve_ = std::make_unique<LissajousCurve>();
		lissajousCurve_->Initialize();

		calling_ = std::make_unique<Calling>();
		calling_->Initialize();
	}

	void DrawManager::AllDraw(PipelineType pipelineType, const ViewProjection& viewProjection, const TileBasedRendering& tileBasedRendering)
	{
		lissajousCurve_->Update();

		//不透明

		//SkinClusterUpdate
		skinningPipeline_->PreDispatch(*commandContext_);
		for (auto& call : calls[kSkinning]) {
			call();
		}

		//drawCall最大値チェック
		assert(drawCallNum_ < kMaxDrawCall);
		drawCallNum_ = 0;

		spritePipeline_->PreDraw(*commandContext_);
		for (auto& call : calls[kPreSprite]) {
			call();
		}

		modelPipeline_->PreDraw(pipelineType, *commandContext_, viewProjection, tileBasedRendering);
		for (auto& call : calls[kModel]) {
			call();
		}

		meshletModelPipeline_->PreDraw(pipelineType, *commandContext_, viewProjection, *calling_->GetCurrentViewProjection(), tileBasedRendering);
		for (auto& call : calls[kMeshletModel]) {
			call();
		}

		glitchMeshletModelPipeline_->PreDraw(pipelineType, *commandContext_, viewProjection, *calling_->GetCurrentViewProjection(), tileBasedRendering);
		for (auto& call : calls[kGlitchMeshletModel]) {
			call();
		}

		meshletEnvironmentMapPipeline_->PreDraw(pipelineType, *commandContext_, viewProjection, *calling_->GetCurrentViewProjection(), tileBasedRendering);
		for (auto& call : calls[kMeshletEnvironmentMap]) {
			call();
		}

		particlePipeline_->PreDraw(pipelineType, *commandContext_, viewProjection, tileBasedRendering);
		for (auto& call : calls[kParticle]) {
			call();
		}

		particleModelPipeline_->PreDraw(pipelineType, *commandContext_, viewProjection, tileBasedRendering);
		for (auto& call : calls[kParticleModel]) {
			call();
		}

		waveModelPipeline_->PreDraw(pipelineType, *commandContext_, viewProjection, *calling_->GetCurrentViewProjection(), tileBasedRendering);
		for (auto& call : calls[kWave]) {
			call();
		}

		//背景
		skyPipeline_->PreDraw(*commandContext_, viewProjection);
		for (auto& call : calls[kSky]) {
			call();
		}

		//透明
		particleModelPipeline_->PreDraw(pipelineType, *commandContext_, viewProjection, tileBasedRendering);
		for (auto& call : calls[kPostParticleModel]) {
			call();
		}

		floorPipeline_->PreDraw(pipelineType, *commandContext_, viewProjection, *calling_->GetCurrentViewProjection(), tileBasedRendering, *lissajousCurve_.get());
		for (auto& call : calls[kFloor]) {
			call();
		}

		raytracing_->Update(*commandContext_);

	}

	void DrawManager::PostSpriteDraw()
	{
		spritePipeline_->PreDraw(*commandContext_);
		for (auto& call : calls[kPostSprite]) {
			call();
		}
	}

	void DrawManager::ShadowDraw()
	{
		shadowPipeline_->PreDraw(*commandContext_, *lightManager_->directionalLights_);
		for (auto& call : calls[kShadow]) {
			call();
		}
	}

	void DrawManager::ShadowSpotLightDraw()
	{

		spotLightShadowPipeline_->PreDraw(*commandContext_, *lightManager_->shadowSpotLights_);
		for (auto& call : calls[kSpotLightShadow]) {
			call();
		}
	}

	void DrawManager::ResetCalls()
	{
		for (int i = 0; i < kCallNum; i++) {
			calls[i].clear();
		}
	}

	void DrawManager::DrawModel(const WorldTransform& worldTransform, const uint32_t modelHandle, const uint32_t textureHandle, const Material& material)
	{
		drawCallNum_++;
		if (calling_->IsDraw(modelHandle, worldTransform)) {
			for (Mesh& mesh : modelManager_->GetModelData(modelHandle).meshes) {
				raytracing_->AddInstanceDesc(mesh.blasBuffer_, worldTransform);
			}
			calls[kModel].push_back([&, modelHandle, textureHandle]() {modelPipeline_->Draw(*commandContext_, modelHandle, worldTransform, material, textureHandle); });
		}
	}

	void DrawManager::DrawModel(const WorldTransform& worldTransform, const uint32_t modelHandle, SkinCluster& skinCluster, const uint32_t textureHandle, const Material& material)
	{
		drawCallNum_++;
		if (calling_->IsDraw(modelHandle, worldTransform)) {
			calls[kSkinning].push_back([&, modelHandle]() {skinningPipeline_->Dispatch(*commandContext_, modelHandle, skinCluster); });
			calls[kModel].push_back([&, modelHandle, textureHandle]() {modelPipeline_->Draw(*commandContext_, modelHandle, worldTransform, skinCluster, material, textureHandle); });
		}
	}

	void DrawManager::DrawMeshletModel(const WorldTransform& worldTransform, const uint32_t modelHandle, const uint32_t textureHandle, const Material& material)
	{
		drawCallNum_++;
		if (calling_->IsDraw(modelHandle, worldTransform)) {
			for (Mesh& mesh : modelManager_->GetModelData(modelHandle).meshes) {
				raytracing_->AddInstanceDesc(mesh.blasBuffer_, worldTransform);
			}
			calls[kMeshletModel].push_back([&, modelHandle, textureHandle]() {meshletModelPipeline_->Draw(*commandContext_, modelHandle, worldTransform, material, textureHandle); });
		}
	}



	void DrawManager::DrawMeshletModel(const WorldTransform& worldTransform, const uint32_t modelHandle, SkinCluster& skinCluster, const uint32_t textureHandle, const Material& material)
	{
		drawCallNum_++;
		if (calling_->IsDraw(modelHandle, worldTransform)) {
			calls[kSkinning].push_back([&, modelHandle]() {skinningPipeline_->Dispatch(*commandContext_, modelHandle, skinCluster); });
			calls[kMeshletModel].push_back([&, modelHandle, textureHandle]() {meshletModelPipeline_->Draw(*commandContext_, modelHandle, worldTransform, skinCluster, material, textureHandle); });
		}
	}

	void DrawManager::DrawGlitchMeshletModel(const WorldTransform& worldTransform, const uint32_t modelHandle, const uint32_t textureHandle, const Material& material)
	{
		drawCallNum_++;
		if (calling_->IsDraw(modelHandle, worldTransform)) {
			for (Mesh& mesh : modelManager_->GetModelData(modelHandle).meshes) {
				raytracing_->AddInstanceDesc(mesh.blasBuffer_, worldTransform);
			}
			calls[kGlitchMeshletModel].push_back([&, modelHandle, textureHandle]() {glitchMeshletModelPipeline_->Draw(*commandContext_, modelHandle, worldTransform, material, textureHandle); });
		}
	}

	void DrawManager::DrawEnvironmentMapMeshletModel(const WorldTransform& worldTransform, const uint32_t modelHandle, const uint32_t textureHandle, const Material& material)
	{
		drawCallNum_++;
		if (calling_->IsDraw(modelHandle, worldTransform)) {
			for (Mesh& mesh : modelManager_->GetModelData(modelHandle).meshes) {
				raytracing_->AddInstanceDesc(mesh.blasBuffer_, worldTransform);
			}
			calls[kMeshletEnvironmentMap].push_back([&, modelHandle, textureHandle]() {meshletEnvironmentMapPipeline_->Draw(*commandContext_, modelHandle, worldTransform, material); });
		}
	}

	void DrawManager::DrawEnvironmentMapMeshletModel(const WorldTransform& worldTransform, const uint32_t modelHandle, SkinCluster& skinCluster, const uint32_t textureHandle, const Material& material)
	{
		drawCallNum_++;
		if (calling_->IsDraw(modelHandle, worldTransform)) {
			calls[kSkinning].push_back([&, modelHandle]() {skinningPipeline_->Dispatch(*commandContext_, modelHandle, skinCluster); });
			calls[kMeshletEnvironmentMap].push_back([&, modelHandle, textureHandle]() {meshletEnvironmentMapPipeline_->Draw(*commandContext_, modelHandle, worldTransform, skinCluster, material); });
		}
	}

	void DrawManager::DrawParticle(ParticleData& bufferData, const uint32_t textureHandle, const Material& material)
	{
		drawCallNum_++;
		calls[kParticle].push_back([&, textureHandle]() {particlePipeline_->Draw(*commandContext_, bufferData, material, textureHandle); });
	}

	void DrawManager::DrawParticleModel(ParticleModelData& bufferData, const uint32_t modelHandle, const Material& material)
	{
		drawCallNum_++;
		for (Mesh& mesh : modelManager_->GetModelData(modelHandle).meshes) {
			for (uint32_t i = 0; i < bufferData.GetDataNum(); i++) {
				raytracing_->AddInstanceDesc(mesh.blasBuffer_, bufferData.GetData()[i].matWorld);
			}
		}
		calls[kParticleModel].push_back([&, modelHandle]() {particleModelPipeline_->Draw(*commandContext_, bufferData, material, modelHandle); });
	}

	void DrawManager::DrawPostParticleModel(ParticleModelData& bufferData, const uint32_t modelHandle, const Material& material)
	{
		drawCallNum_++;
		calls[kPostParticleModel].push_back([&, modelHandle]() {particleModelPipeline_->Draw(*commandContext_, bufferData, material, modelHandle); });
	}

	void DrawManager::DrawPreSprite(SpriteData& spriteData)
	{
		drawCallNum_++;
		calls[kPreSprite].push_back([&]() {spritePipeline_->Draw(*commandContext_, spriteData); });
	}

	void DrawManager::DrawPostSprite(SpriteData& spriteData)
	{
		drawCallNum_++;
		calls[kPostSprite].push_back([&]() {spritePipeline_->Draw(*commandContext_, spriteData); });
	}

	void DrawManager::DrawShadow(const WorldTransform& worldTransform, const uint32_t modelHandle)
	{
		drawCallNum_++;
		calls[kShadow].push_back([&, modelHandle]() {shadowPipeline_->Draw(*commandContext_, modelHandle, worldTransform); });
	}

	void DrawManager::DrawSpotLightShadow(const WorldTransform& worldTransform, const uint32_t modelHandle)
	{
		drawCallNum_++;
		calls[kSpotLightShadow].push_back([&, modelHandle]() {spotLightShadowPipeline_->Draw(*commandContext_, modelHandle, worldTransform); });
	}

	void DrawManager::DrawSky(const WorldTransform& worldTransform)
	{
		drawCallNum_++;
		calls[kSky].push_back([&]() {skyPipeline_->Draw(*commandContext_, worldTransform); });
	}

	void DrawManager::DrawWaveModel(const WorldTransform& worldTransform, const WaveData& waveData, const WaveIndexData& waveIndexData, const uint32_t modelHandle, const uint32_t textureHandle, const Material& material)
	{
		drawCallNum_++;
		if (calling_->IsDraw(modelHandle, worldTransform)) {
			for (Mesh& mesh : modelManager_->GetModelData(modelHandle).meshes) {
				raytracing_->AddInstanceDesc(mesh.blasBuffer_, worldTransform);
			}
			calls[kWave].push_back([&, modelHandle, textureHandle]() {waveModelPipeline_->Draw(*commandContext_, modelHandle, worldTransform, waveData, waveIndexData, material, textureHandle); });
		}
	}

	void DrawManager::DrawWaveModel(const WorldTransform& worldTransform, const WaveData& waveData, const WaveIndexData& waveIndexData, const uint32_t modelHandle, SkinCluster& skinCluster, const uint32_t textureHandle, const Material& material)
	{
		drawCallNum_++;
		if (calling_->IsDraw(modelHandle, worldTransform)) {
			calls[kSkinning].push_back([&, modelHandle]() {skinningPipeline_->Dispatch(*commandContext_, modelHandle, skinCluster); });
			calls[kWave].push_back([&, modelHandle, textureHandle]() {waveModelPipeline_->Draw(*commandContext_, modelHandle, worldTransform, waveData, waveIndexData, skinCluster, material, textureHandle); });
		}
	}

	void DrawManager::DrawFloor(const WorldTransform& worldTransform, const WaveData& waveData, const WaveIndexData& waveIndexData, const uint32_t modelHandle)
	{
		drawCallNum_++;
		for (Mesh& mesh : modelManager_->GetModelData(modelHandle).meshes) {
			raytracing_->AddInstanceDesc(mesh.blasBuffer_, worldTransform);
		}
		calls[kFloor].push_back([&, modelHandle]() {floorPipeline_->Draw(*commandContext_, modelHandle, worldTransform, waveData, waveIndexData); });
	}

}
