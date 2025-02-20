/**
 * @file AnimationManager.cpp
 * @brief Animationに必要な関数などをラップしたもの
 */
#include "Animation/AnimationManager.h"

#include <DirectXTex.h>
#include <cassert>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Graphics/DirectXCommon.h"
#include "Texture/TextureManager.h"

using namespace DirectX;

namespace Engine {

	const Animation& AnimationManager::Load(const std::string& fileName, const std::string& animationName) {
		return AnimationManager::GetInstance()->LoadInternal(fileName, animationName);
	}

	void AnimationManager::CreateAnimations(const std::string& fileName, AnimationData& animationData, const std::string& animationName)
	{

		//拡張子がない名前を取得
		std::size_t dotPos = fileName.find_last_of('.');
		std::string n = fileName.substr(0, dotPos);

		std::string directoryPath = "Resources/animations/" + n + "/";

		Assimp::Importer importer;
		std::string filePath = directoryPath + fileName;
		const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
		assert(scene->mNumAnimations != 0); //animationがない
		aiAnimation* animationAssimp = nullptr;
		for (uint32_t animationIndex = 0; animationIndex < scene->mNumAnimations; animationIndex++) {
			aiString name = scene->mAnimations[animationIndex]->mName;
			if (name.C_Str() == animationName) {
				animationAssimp = scene->mAnimations[animationIndex];//名前が一致したら代入
				break;
			}
		}

		assert(animationAssimp != nullptr);
		animationData.animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);

		for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
			aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			NodeAnimation& nodeAnimation = animationData.animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
				KeyframeVector3 keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { -keyAssimp.mValue.x ,keyAssimp.mValue.y,keyAssimp.mValue.z };
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

	void AnimationManager::ApplyAnimation(Skeleton& skeleton, Animation& animation, float animationTime)
	{
		for (Joint& joint : skeleton.GetAllJoint()) {
			// 対称のJointのAnimationがあれば、値の適用を行う。下記のif文はC+;17から可能になった初期化付きif文。
			if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
				const NodeAnimation& rootNodeAnimation = (*it).second;
				joint.transform.translation_ = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
				joint.transform.quaternion_ = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
				joint.transform.scale_ = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
			}
		}
	}

	void AnimationManager::LerpSkeleton(float t, Skeleton& skeleton, const Animation& fromAnimation, float fromAnimationTime, const Animation& toAnimation, float toAnimationTime)
	{
		for (Joint& joint : skeleton.GetAllJoint()) {
			auto fromIt = fromAnimation.nodeAnimations.find(joint.name);
			auto toIt = toAnimation.nodeAnimations.find(joint.name);
			if (fromIt != fromAnimation.nodeAnimations.end() && toIt != toAnimation.nodeAnimations.end()) {
				const NodeAnimation& fromNodeAnimation = (*fromIt).second;
				const NodeAnimation& toNodeAnimation = (*toIt).second;
				joint.transform.translation_ = Lerp(t, CalculateValue(fromNodeAnimation.translate.keyframes, fromAnimationTime), CalculateValue(toNodeAnimation.translate.keyframes, toAnimationTime));
				joint.transform.quaternion_ = Slerp(t, CalculateValue(fromNodeAnimation.rotate.keyframes, fromAnimationTime), CalculateValue(toNodeAnimation.rotate.keyframes, toAnimationTime));
				joint.transform.scale_ = Lerp(t, CalculateValue(fromNodeAnimation.scale.keyframes, fromAnimationTime), CalculateValue(toNodeAnimation.scale.keyframes, toAnimationTime));
			}
		}
	}

	const Animation& AnimationManager::LoadInternal(const std::string& fileName, const std::string& animationName) {

		assert(useAnimationCount_ < kNumAnimations);
		uint32_t handle = useAnimationCount_;

		// 読み込み済みanimationを検索
		const auto& it = std::find_if(animations_->begin(), animations_->end(), [&](const auto& animation) {return animation.name == fileName; });

		if (it != animations_->end()) {
			// 読み込み済みanimationの要素番号を取得
			handle = static_cast<uint32_t>(std::distance(animations_->begin(), it));
			auto& animationData = animations_->at(handle);
			return animationData.animation;
		}

		// 書き込むAnimationの参照
		auto& animationData = (animations_->at(handle));

		animationData.name = fileName;

		CreateAnimations(fileName, animationData, animationName);

		useAnimationCount_++;
		return animationData.animation;
	}
}