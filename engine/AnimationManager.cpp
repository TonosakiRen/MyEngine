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

const Animation& AnimationManager::Load(const std::string& fileName) {
	return AnimationManager::GetInstance()->LoadInternal(fileName);
}

void AnimationManager::CreateAnimations(AnimationData& animationData)
{
	HRESULT result = S_FALSE;

	//拡張子がない名前を取得
	std::size_t dotPos = animationData.name.find_last_of('.');
	std::string n = animationData.name.substr(0, dotPos);

	std::string directoryPath = "Resources/animations/" + n + "/";

	Assimp::Importer importer;
	std::string filePath = directoryPath + animationData.name;
	const aiScene* scene = importer.ReadFile(filePath.c_str(),0);
	assert(scene->mNumAnimations != 0); //animationがない
	aiAnimation* animationAssimp = scene->mAnimations[0];//最初のanimationだけ採用。もちろん複数対応するに越したことはない。
	animationData.animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);
	
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animationData.animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { -keyAssimp.mValue.x ,-keyAssimp.mValue.y,-keyAssimp.mValue.z };
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x ,-keyAssimp.mValue.y,-keyAssimp.mValue.z,keyAssimp.mValue.w };
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x ,keyAssimp.mValue.y,keyAssimp.mValue.z };
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}
	}
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

void AnimationManager::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime)
{
	for (Joint& joint : skeleton.joints) {
		// 対称のJointのAnimationがあれば、値の適用を行う。下記のif文はC+;17から可能になった初期化付きif文。
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translation_ = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
			joint.transform.quaternion_ = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale_ = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
		}
	}
}

const Animation& AnimationManager::LoadInternal(const std::string& name) {

	assert(useAnimationCount_ < kNumAnimations);
	uint32_t handle = useAnimationCount_;

	// 読み込み済みanimationを検索
	const auto& it = std::find_if(animations_->begin(), animations_->end(), [&](const auto& texture) {return texture.name == name; });

	if (it != animations_->end()) {
		// 読み込み済みanimationの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(animations_->begin(), it));
		auto& animationData = animations_->at(handle);
		return animationData.animation;
	}

	// 書き込むAnimationの参照
	auto& animationData = (animations_->at(handle));

	animationData.name = name;

	CreateAnimations(animationData);

	useAnimationCount_++;
	return animationData.animation;
}