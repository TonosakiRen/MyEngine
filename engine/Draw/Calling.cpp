#include "Draw/DrawManager.h"

#include <Windows.h>
#include <DirectXTex.h>
#include <cassert>

#include "Draw/Calling.h"
#include "Graphics/DirectXCommon.h"
#include "Light/ShaderManager.h"

using namespace Microsoft::WRL;

void Calling::Initialize()
{
	modelManager_ = Engine::ModelManager::GetInstance();
}

bool Calling::IsDraw(const uint32_t modelHandle,const WorldTransform& worldTransform)
{
	Sphere sphere = modelManager_->GetModelSphere(modelHandle);
	sphere.center = sphere.center * worldTransform.matWorld_;
	sphere.radius *= worldTransform.maxScale_;

	Frustum frustum = currentViewProjection_->GetWorldFrustum();

	return(IsFrustumSphereCollision(frustum,sphere));
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