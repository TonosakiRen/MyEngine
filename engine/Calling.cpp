#include "DrawManager.h"
#include <DirectXTex.h>
#include <cassert>
#include "Calling.h"
#include "DirectXCommon.h"
#include "ShaderManager.h"

#include <Windows.h>
using namespace Microsoft::WRL;

void Calling::Initialize()
{
	modelManager = ModelManager::GetInstance();
}

bool Calling::isDraw(const uint32_t modelHandle,const WorldTransform& worldTransform)
{
	Sphere sphere = modelManager->GetModelSphere(modelHandle);
	sphere.center = sphere.center * worldTransform.matWorld_;
	sphere.radius *= worldTransform.maxScale_;

	Frustum frustum = currentViewProjection->GetWorldFrustum();

	return(IsFrustumSphereCollision(frustum,sphere));
}

uint32_t Calling::GetTileIndex()
{
	return 0;
}

bool Calling::IsFrustumSphereCollision(const Frustum& frustum, const Sphere& sphere)
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