#pragma once
/**
 * @file LightManager.h
 * @brief LightをまとめているClass
 */
#include "Mymath.h"
#include <vector>
#include <memory>
#include "Light/AreaLights.h"
#include "Light/DirectionalLights.h"
#include "Light/LightNumBuffer.h"
#include "Light/PointLights.h"
#include "Light/ShadowSpotLights.h"
#include "Light/SpotLights.h"
class LightManager
{
public:

	static LightManager* GetInstance();

	void Initialize();
	void Update();
	void Finalize();

	std::unique_ptr<AreaLights> areaLights_;
	std::unique_ptr<DirectionalLights> directionalLights_;
	std::unique_ptr<LightNumBuffer> lightNumBuffer_;
	std::unique_ptr<PointLights> pointLights_;
	std::unique_ptr<ShadowSpotLights> shadowSpotLights_;
	std::unique_ptr<SpotLights> spotLights_;

private:
	LightManager() = default;
	~LightManager() = default;
	LightManager(const LightManager&) = delete;
	LightManager& operator=(const LightManager&) = delete;
};