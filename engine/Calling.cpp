#include "DrawManager.h"
#include <DirectXTex.h>
#include <cassert>
#include "Calling.h"

void Calling::Initialize()
{
	modelManager = ModelManager::GetInstance();
}

bool Calling::isDraw(const uint32_t modelHandle,const WorldTransform& worldTransform)
{
	Sphere sphere = modelManager->GetModelSphere(modelHandle);
	sphere.center += MakeTranslation(worldTransform.matWorld_);
	Vector3 scale = MakeScale(worldTransform.matWorld_);
	float maxScale = scale.x;
	if (scale.y > maxScale) {
		if (scale.z > scale.y) {
			maxScale = scale.z;
		}
		else {
			maxScale = scale.y;
		}
	}
	sphere.radius *= maxScale;

	Frustum frustum = currentViewProjection->GetWorldFrustum();

	return(isFrustumSphereCollision(frustum,sphere));
}

uint32_t Calling::GetTileIndex()
{
	return 0;
}

bool Calling::isFrustumSphereCollision(const Frustum& frustum, const Sphere& sphere)
{
	//平面の法線と内積をとる
	for (uint32_t i = 0; i < 6; i++) {
		//プラスであれば外側距離を測る,内側の場合マイナス
		float a = Dot(frustum.plane[i].normal, sphere.center) - frustum.plane[i].distance;
		if (a > sphere.radius) {
			return false;
		}
	}
	return true;
}
