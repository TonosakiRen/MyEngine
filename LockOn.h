#pragma once
#include "Boss.h"
#include "Input.h"
#include "Sprite.h"
class LockOn
{
public:
	void Initialize();
	void Update(const std::list<std::unique_ptr<Boss>>& enemies, const ViewProjection& viewProjection);
	void SearchTarget(const std::list<std::unique_ptr<Boss>>& enemies, const ViewProjection& viewProjection);
	void Draw();

	void SearchTarget();

	bool OutOfRangeTarget(const ViewProjection& viewProjection);

	const Vector3 GetTargetPosition() const { return MakeTranslation(target_->GetWorldTransform()->matWorld_); }

	bool ExitTarget() const { return target_ ? true : false; }

	Input* input_ = nullptr;

	const Boss* target_ = nullptr;

	float minDistance_ = 10.0f;
	float maxDistance_ = 60.0f;
	float angleTange_ = Radian(60.0f);

	std::unique_ptr<Sprite> reticle_;

	bool isLookOn_ = false;

};

