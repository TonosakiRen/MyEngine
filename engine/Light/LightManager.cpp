/**
 * @file LightManager.cpp
 * @brief LightをまとめているClass
 */
#include "Light/LightManager.h"
#include "Graphics/DirectXCommon.h"

LightManager* LightManager::GetInstance()
{
	static LightManager instance;
	return &instance;
}

void LightManager::Finalize()
{
	areaLights_.reset();
	directionalLights_.reset();
	lightNumBuffer_.reset();
	pointLights_.reset();
	shadowSpotLights_.reset();
	spotLights_.reset();
}

void LightManager::Initialize()
{
	areaLights_ = std::make_unique<AreaLights>();
	areaLights_->Initialize();
	directionalLights_ = std::make_unique<DirectionalLights>();
	directionalLights_->Initialize();
	lightNumBuffer_ = std::make_unique<LightNumBuffer>();
	lightNumBuffer_->Initialize();
	pointLights_ = std::make_unique<PointLights>();
	pointLights_->Initialize();
	shadowSpotLights_ = std::make_unique<ShadowSpotLights>();
	shadowSpotLights_->Initialize();
	spotLights_ = std::make_unique<SpotLights>();
	spotLights_->Initialize();
}

void LightManager::Update() {

	areaLights_->Update();
	directionalLights_->Update();
	pointLights_->Update();
	shadowSpotLights_->Update();
	spotLights_->Update();
}