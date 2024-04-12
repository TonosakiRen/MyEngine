#include "AnimationManager.h"
#include <DirectXTex.h>
#include <cassert>
#include "DirectXCommon.h"
#include "TextureManager.h"
#include <fstream>
#include <sstream>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace DirectX;

uint32_t AnimationManager::Load(const std::string& fileName) {
	return AnimationManager::GetInstance()->LoadInternal(fileName);
}

void AnimationManager::CreateAnimations(AnimationData& animationIndex)
{
	HRESULT result = S_FALSE;

	std::string directoryPath = "Resources/animations/" + animationIndex.name + "/";

	Assimp::Importer importer;
	std::string filePath = directoryPath + animationIndex.name + ".obj";
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);

}

void AnimationManager::Finalize()
{
	animations_.reset();
}


AnimationManager* AnimationManager::GetInstance() {
	static AnimationManager instance;
	if (!instance.animations_) {
		instance.animations_ = std::make_unique<std::array<AnimationData, kNumAnimations>>();
	}
	return &instance;
}

uint32_t AnimationManager::LoadInternal(const std::string& name) {

	assert(useAnimationCount_ < kNumAnimations);
	uint32_t handle = useAnimationCount_;

	// 読み込み済みmodelを検索
	auto it = std::find_if(animations_->begin(), animations_->end(), [&](const auto& texture) {return texture.name == name; });

	if (it != animations_->end()) {
		// 読み込み済みmodelの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(animations_->begin(), it));
		return handle;
	}

	// 書き込むAnimationの参照
	auto& animationIndex = (animations_->at(kNumAnimations));

	animationIndex.name = name;

	CreateAnimations(animationIndex);

	useAnimationCount_++;
	return handle;
}