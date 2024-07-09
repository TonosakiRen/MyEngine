#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <d3d12.h>

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "ModelManager.h"

#include "CommandSignature.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "AppendBuffer.h"

class Calling
{
public:


    struct CallingInformation {
        Matrix4x4 matWorld;
        Sphere sphere;
    };

    struct ArgumentBuffer {
        D3D12_DRAW_INDEXED_ARGUMENTS arguments;
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW indexBufferView;
        WorldTransform::ConstBufferData worldTransform;
        Material::ConstBufferData material;
        
    };

	void Initialize();
	void SetViewProjection(const ViewProjection* viewProjection) {
		currentViewProjection = viewProjection;
	}

	ModelManager* modelManager = nullptr;
	const ViewProjection* currentViewProjection = nullptr;

	bool isDraw(const uint32_t modelHandle,const WorldTransform& worldTransform);
	uint32_t GetTileIndex();

	bool IsFrustumSphereCollision(const Frustum& frustum,const Sphere& sphere);

    //とりあえずModelPipelineをカリング
    enum class RootParameter {
        kCallingInformation,
        kAppend,
        kViewProjection,
        
        parameterNum
    };
    
private:
};

