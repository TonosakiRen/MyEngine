#pragma once
/**
 * @file Calling.h
 * @brief カリングを行うClass
 */
#include <vector>
#include <memory>
#include <functional>
#include <d3d12.h>

#include "GameComponent/ViewProjection.h"
#include "GameComponent/WorldTransform.h"
#include "Model/ModelManager.h"
#include "Graphics/CommandSignature.h"
#include "Graphics/PipelineState.h"
#include "Graphics/RootSignature.h"

class Calling
{
public:

    //とりあえずModelPipelineをカリング
    enum class RootParameter {
        kCallingInformation,
        kAppend,
        kViewProjection,
        
        parameterNum
    };

    //カリングのための情報
    struct CallingInformation {
        Matrix4x4 matWorld;
        Sphere sphere;
    };

	void Initialize();
    //カリングするViewProjectionのセット
	void SetViewProjection(const ViewProjection* viewProjection) {
		currentViewProjection_ = viewProjection;
	}
    //描画するかしないかの判定する関数
	bool IsDraw(const uint32_t modelHandle,const WorldTransform& worldTransform);
	uint32_t GetTileIndex();
	bool IsFrustumSphereCollision(const Frustum& frustum,const Sphere& sphere);

    
	ModelManager* modelManager_ = nullptr;
	const ViewProjection* currentViewProjection_ = nullptr;
private:
};

