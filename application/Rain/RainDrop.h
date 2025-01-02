#pragma once
/**
 * @file RainDrop.h
 * @brief 雨粒
 */
#include "MyMath.h"
#include "GameComponent/Collider.h"
#include "GameComponent/GameObject.h"
#include "Light/PointLights.h"
#include "Particle/ExplodeParticle.h"

class RainDrop : public GameObject
{
friend class RainManager;
public:
	static Vector3 modelSize;
	static const uint32_t anActiveFrame_ = 360;

	void Initialize(uint32_t modelHandle, PointLight* pointLight, ExplodeParticle* explodeParticle, const Vector3& position, const Vector4& color);
	void Finalize();
	void Update();
	void OnCollision();

	//Getter
	bool IsActive() const { return isActive_; }
	Collider& GetCollider() { return collider_; }
	const Vector4& GetColor() { return color_; }

private:
	PointLight* pointLight_ = nullptr;
	ExplodeParticle* explodeParticle_ = nullptr;

	const float speed_ = 0.4f;
	const float boundNum_ = 0.6f;
	const float radius_ = 6.0f;
	const float intensity_ = 3.0f;

	Collider collider_;
	bool isActive_ = false;
	bool isDrop_ = false;
	Vector3 velocity_;
	Vector3 acceleration_;
	Vector4 color_;
	uint32_t dropFrame_ = anActiveFrame_;

};

