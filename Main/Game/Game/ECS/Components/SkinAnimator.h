#pragma once

#include <memory>
#include <unordered_map>

class AnimationClip;

// requires also SkinnedMeshRenderer
class SkinAnimator {
	friend class SkinAnimatorSystem;

private:
	std::shared_ptr<AnimationClip> currentClip;

public:
	std::string currentClipName;
	float animTime = 0.0f;
	std::unordered_map<std::string, std::shared_ptr<AnimationClip>> clips;

	void SetAnimation(std::string name)
	{
		if (currentClipName != name)
		{
			currentClipName = name;
			currentClip = clips[name];
			animTime = 0.0f;
		}
	}
};