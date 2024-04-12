#pragma once

#include <array>
#include "CommandContext.h"
#include <string>
#include <unordered_map>
#include <wrl.h>
#include <vector>
#include "Mesh.h"
#include "DescriptorHandle.h"
#include "UploadBuffer.h"
#include <memory>

class AnimationManager
{
public:
	static const size_t kNumAnimations = 256;

	struct AnimationData {
		std::string name;
	};


	static uint32_t Load(const std::string& fileName);

	static AnimationManager* GetInstance();

	void Finalize();
private:
	AnimationManager() = default;
	~AnimationManager() = default;
	AnimationManager(const AnimationManager&) = delete;
	AnimationManager& operator=(const AnimationManager&) = delete;

	std::unique_ptr<std::array<AnimationData, kNumAnimations>> animations_;
	uint32_t useAnimationCount_ = 0;

	uint32_t LoadInternal(const std::string& fileName);

	void CreateAnimations(AnimationData& modelIndex);

};

