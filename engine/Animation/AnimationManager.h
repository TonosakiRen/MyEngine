#pragma once
/**
 * @file AnimationManager.h
 * @brief Animationに必要な関数などをラップしたもの
 */
#include <array>
#include <string>
#include <unordered_map>
#include <wrl.h>
#include <vector>
#include <memory>
#include <map>

#include "Mesh/Mesh.h"
#include "Graphics/CommandContext.h"
#include "Graphics/DescriptorHandle.h"
#include "GPUResource/UploadBuffer.h"
#include "Model/ModelManager.h"
#include "Animation/Skeleton.h"

template <typename tValue>
struct Keyframe {
	float time; //キーフレームの値
	tValue value; //キーフレームの時刻(秒)
};

using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

template <typename tValue>
struct AnimationCurve {
	std::vector<Keyframe<tValue>> keyframes;
};

struct NodeAnimation {
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};

struct Animation {
	float duration; //全体の尺(秒)
	//NodeAnimationの集合。Node名で開けるようにしておく。
	std::map<std::string, NodeAnimation> nodeAnimations;
};

//keyFrameから時間で数値を算出
inline Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time) {
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size(); ++index) {
		size_t nextIndex = index + 1;
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(t,keyframes[index].value, keyframes[nextIndex].value);
		}
	}

	return (*keyframes.rbegin()).value;
}

//keyFrameから時間で数値を算出
inline Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time) {
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size(); ++index) {
		size_t nextIndex = index + 1;
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Slerp(t, keyframes[index].value, keyframes[nextIndex].value);
		}
	}

	return (*keyframes.rbegin()).value;
}

namespace Engine {

	class AnimationManager
	{
	public:
		//Animationの数
		static const size_t kNumAnimations = 256;

		struct AnimationData {
			std::string name;
			Animation animation;
		};

		//Animationのロード
		static const Animation& Load(const std::string& fileName, const std::string& animationName);

		static AnimationManager* GetInstance();

		//Animationの適用
		static void ApplyAnimation(Skeleton& skeleton, Animation& animation, float animationTime);

		//Animationのラープ
		static void LerpSkeleton(float t, Skeleton& skeleton, const Animation& fromAnimation, float fromAnimationTime, const Animation& toAnimation, float toAnimationTime);

		void Finalize();
	private:
		AnimationManager() = default;
		~AnimationManager() = default;
		AnimationManager(const AnimationManager&) = delete;
		AnimationManager& operator=(const AnimationManager&) = delete;

		std::unique_ptr<std::array<AnimationData, kNumAnimations>> animations_;
		uint32_t useAnimationCount_ = 0;

		const Animation& LoadInternal(const std::string& fileName, const std::string& animationName);

		void CreateAnimations(const std::string& fileName, AnimationData& modelIndex,const std::string& animationName);

	};

}

