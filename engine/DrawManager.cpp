#include "DrawManager.h"
#include <DirectXTex.h>
#include <cassert>
#include "Renderer.h"

using namespace DirectX;

std::unique_ptr<Material> DrawManager::defaultMaterial_;

void DrawManager::Finalize()
{
	modelPipeline_.reset();
	meshletModelPipeline_.reset();
	skinningPipeline_.reset();
	particlePipeline_.reset();
	particleModelPipeline_.reset();
	spritePipeline_.reset();
	shadowPipeline_.reset();
	spotLightShadowPipeline_.reset();
	skyPipeline_.reset();
	defaultMaterial_.reset();
	floorPipeline_.reset();

	calls->clear();
}


DrawManager* DrawManager::GetInstance() {
	static DrawManager instance;
	return &instance;
}

void DrawManager::Initialize(CommandContext& CommandContext)
{
	lightManager_ = LightManager::GetInstance();

	commandContext_ = &CommandContext;
	defaultMaterial_ = std::make_unique<Material>();
	defaultMaterial_->Initialize();
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
	floorPipeline_ = std::make_unique<FloorRenderer>();
	floorPipeline_->Initialize(CommandContext);

	calling_ = std::make_unique<Calling>();
	calling_->Initialize();
}

void DrawManager::AllDraw(const ViewProjection& viewProjection)
{

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

	modelPipeline_->PreDraw(*commandContext_, viewProjection);
	for (auto& call : calls[kModel]) {
		call();
	}

	meshletModelPipeline_->PreDraw(*commandContext_, viewProjection, *calling_->currentViewProjection);
	for (auto& call : calls[kMeshletModel]) {
		call();
	}

	particlePipeline_->PreDraw(*commandContext_, viewProjection);
	for (auto& call : calls[kParticle]) {
		call();
	}

	particleModelPipeline_->PreDraw(*commandContext_, viewProjection);
	for (auto& call : calls[kParticleModel]) {
		call();
	}

	skyPipeline_->PreDraw(*commandContext_, viewProjection);
	for (auto& call : calls[kSky]) {
		call();
	}

	floorPipeline_->PreDraw(*commandContext_, viewProjection);
	for (auto& call : calls[kFloor]) {
		call();
	}

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

void DrawManager::DrawModel(const WorldTransform& worldTransform, const uint32_t modelHandle,const uint32_t textureHandle, const Material& material)
{
	drawCallNum_++;
	if (calling_->isDraw(modelHandle,worldTransform)) {
		calls[kModel].push_back([&, modelHandle , textureHandle]() {modelPipeline_->Draw(*commandContext_, modelHandle, worldTransform, material, textureHandle); });
	}
}

void DrawManager::DrawModel(const WorldTransform& worldTransform, const uint32_t modelHandle, SkinCluster& skinCluster, const uint32_t textureHandle, const Material& material)
{
	drawCallNum_++;
	if (calling_->isDraw(modelHandle, worldTransform)) {
		calls[kSkinning].push_back([&, modelHandle]() {skinningPipeline_->Dispatch(*commandContext_,modelHandle,skinCluster); });
		calls[kModel].push_back([&, modelHandle, textureHandle]() {modelPipeline_->Draw(*commandContext_, modelHandle, worldTransform, skinCluster, material, textureHandle); });
	}
}

void DrawManager::DrawMeshletModel(const WorldTransform& worldTransform, const uint32_t modelHandle, const uint32_t textureHandle, const Material& material)
{
	drawCallNum_++;
	if (calling_->isDraw(modelHandle, worldTransform)) {
		calls[kMeshletModel].push_back([&, modelHandle, textureHandle]() {meshletModelPipeline_->Draw(*commandContext_, modelHandle, worldTransform, material, textureHandle); });
	}
}

void DrawManager::DrawMeshletModel(const WorldTransform& worldTransform, const uint32_t modelHandle, SkinCluster& skinCluster, const uint32_t textureHandle, const Material& material)
{
	drawCallNum_++;
	if (calling_->isDraw(modelHandle, worldTransform)) {
		calls[kSkinning].push_back([&, modelHandle]() {skinningPipeline_->Dispatch(*commandContext_, modelHandle, skinCluster); });
		calls[kMeshletModel].push_back([&, modelHandle, textureHandle]() {meshletModelPipeline_->Draw(*commandContext_, modelHandle, worldTransform, skinCluster, material, textureHandle); });
	}
}

void DrawManager::DrawParticle(ParticleData& bufferData, const uint32_t textureHandle, const Material& material)
{
	drawCallNum_++;
	calls[kParticle].push_back([&, textureHandle]() {particlePipeline_->Draw(*commandContext_, bufferData,material, textureHandle); });
}

void DrawManager::DrawParticleModel(ParticleModelData& bufferData, const uint32_t modelHandle, const Material& material)
{
	drawCallNum_++;
	calls[kParticleModel].push_back([&, modelHandle]() {particleModelPipeline_->Draw(*commandContext_, bufferData, material, modelHandle); });
}

void DrawManager::DrawPreSprite(SpriteData& spriteData)
{
	drawCallNum_++;
	calls[kPreSprite].push_back([&]() {spritePipeline_->Draw(*commandContext_,spriteData); });
}

void DrawManager::DrawPostSprite(SpriteData& spriteData)
{
	drawCallNum_++;
	calls[kPostSprite].push_back([&]() {spritePipeline_->Draw(*commandContext_, spriteData); });
}

void DrawManager::DrawShadow(const WorldTransform& worldTransform, const uint32_t modelHandle)
{
	drawCallNum_++;
	calls[kShadow].push_back([&, modelHandle]() {shadowPipeline_->Draw(*commandContext_,modelHandle,worldTransform); });
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

void DrawManager::DrawFloor(const WorldTransform& worldTransform, const uint32_t modelHandle)
{
	drawCallNum_++;
	calls[kFloor].push_back([&, modelHandle]() {floorPipeline_->Draw(*commandContext_, modelHandle, worldTransform); });
}

