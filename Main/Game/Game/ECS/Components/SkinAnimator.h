#pragma once

#include <memory>
#include <unordered_map>

class AnimationClip;

// requires also SkinnedMeshRenderer
class SkinAnimator {
	friend class SkinAnimatorSystem;

private:
	std::shared_ptr<AnimationClip> currentClip;
	float animTime = 0.0f;
	std::shared_ptr<AnimationClip> nextClip;
	float nextAnimTime = 0.0f;

	bool transitioning = false;
	float transitionDuration = 0.25f;

public:
	std::string currentClipName;
	std::unordered_map<std::string, std::shared_ptr<AnimationClip>> clips;

	void SetAnimation(std::string name)
	{
		if (currentClipName != name)
		{
			assert(clips.find(name) != clips.end() && "Trying to set non-existing animation clip");

			currentClipName = name;
			currentClip = clips[name];
			animTime = 0.0f;
			transitioning = false;
		}
	}
	void TransitToAnimation(std::string name, float duration = 0.2f)
	{
		if (currentClipName != name)
		{
			assert(clips.find(name) != clips.end() && "Trying to set non-existing animation clip");

			if (transitioning)
			{
				currentClip = nextClip;
				animTime = nextAnimTime;
			}

			currentClipName = name;

			nextClip = clips[name];
			nextAnimTime = 0.0f;
			transitionDuration = duration;
			transitioning = true;
		}
	}
};