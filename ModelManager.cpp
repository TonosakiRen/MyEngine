#include "ModelManager.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <cassert>
#include "DirectXCommon.h"
#include "TextureManager.h"
using namespace DirectX;

uint32_t ModelManager::Load(const std::string& fileName) {
	return ModelManager::GetInstance()->LoadInternal(fileName);
}


ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;
	return &instance;
}

void ModelManager::DrawInstanced(ID3D12GraphicsCommandList* commandList, uint32_t modelHandle, UINT textureRootParamterIndex) {
	assert(modelHandle < kNumModels);
	// srvセット
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureRootParamterIndex, models_[modelHandle].mesh.GetUv());
	// 頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, models_[modelHandle].mesh.GetVbView());
	// 描画コマンド
	commandList->DrawInstanced(models_[modelHandle].mesh.GetSize(), 1, 0, 0);
}

uint32_t ModelManager::LoadInternal(const std::string& name) {

	assert(useModelCount_ < kNumModels);
	uint32_t handle = useModelCount_;

	// 読み込み済みmodelを検索
	auto it = std::find_if(models_.begin(), models_.end(), [&](const auto& texture) {return texture.name == name; });

	if (it != models_.end()) {
		// 読み込み済みmodelの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(models_.begin(), it));
		return handle;
	}

	// 書き込むmodelの参照
	ModelIndex& modelIndex = models_.at(useModelCount_);
	modelIndex.name = name;

	modelIndex.mesh.Create(name);

	useModelCount_++;
	return handle;
}

