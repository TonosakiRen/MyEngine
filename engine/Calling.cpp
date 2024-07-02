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

void Calling::CreatePipeline()
{

	ComPtr<IDxcBlob> uavBlob;

	auto shaderManager = ShaderManager::GetInstance();
	uavBlob = shaderManager->Compile(L"CallingCS.hlsl", ShaderManager::kCompute);
	assert(uavBlob != nullptr);

	CD3DX12_DESCRIPTOR_RANGE ranges[2]{};
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, int(RootParameter::kCallingInformation));
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, int(RootParameter::kAppend));

	CD3DX12_ROOT_PARAMETER rootparams[static_cast<int>(RootParameter::parameterNum)]{};
	rootparams[int(RootParameter::kCallingInformation)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kCallingInformation)]);
	rootparams[int(RootParameter::kAppend)].InitAsDescriptorTable(1, &ranges[int(RootParameter::kAppend)]);
	rootparams[int(RootParameter::kViewProjection)].InitAsConstantBufferView(1, 0);

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.pParameters = rootparams;
	rootSignatureDesc.NumParameters = _countof(rootparams);

	modelCallingRootSignature_.Create(rootSignatureDesc);


	D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
	desc.pRootSignature = modelCallingRootSignature_;
	desc.CS = CD3DX12_SHADER_BYTECODE(uavBlob->GetBufferPointer(), uavBlob->GetBufferSize());

	modelCallingPipeline_.Create(desc);

	callingInformation_.Create(sizeof(CallingInformation),DrawManager::kCallNum);
}
