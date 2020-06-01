#pragma once

#include <memory>
#include <unordered_map>
#include "Resourcing/AnimationClip.h"


// requires also SkinnedMeshRenderer
class SkinAnimator {
	friend class SkinAnimatorSystem;

private:
	std::shared_ptr<AnimationClip> currentClip;
	AnimationClip::PlaybackMode currentClipMode;
	float animTime = 0.0f;

	std::shared_ptr<AnimationClip> nextClip;
	AnimationClip::PlaybackMode nextClipMode;
	float nextAnimTime = 0.0f;

	bool transitioning = false;
	float transitionDuration = 0.25f;

public:
	std::string currentClipName;
	std::unordered_map<std::string, std::shared_ptr<AnimationClip>> clips;

	void SetAnimation(std::string name, AnimationClip::PlaybackMode playbackMode = AnimationClip::PlaybackMode::LOOP)
	{
		if (currentClipName != name)
		{
			assert(clips.find(name) != clips.end() && "Trying to set non-existing animation clip");

			currentClipName = name;
			currentClip = clips[name];
			animTime = 0.0f;
			currentClipMode = playbackMode;
			transitioning = false;
		}
	}
	void TransitToAnimation(std::string name, float duration = 0.2f, AnimationClip::PlaybackMode playbackMode = AnimationClip::PlaybackMode::LOOP)
	{
		if (currentClipName != name)
		{
			assert(clips.find(name) != clips.end() && "Trying to set non-existing animation clip");

			if (transitioning)
			{
				currentClip = nextClip;
				animTime = nextAnimTime;
				currentClipMode = nextClipMode;
			}

			currentClipName = name;

			nextClip = clips[name];
			nextAnimTime = 0.0f;
			nextClipMode = playbackMode;
			transitionDuration = duration;
			transitioning = true;
		}
	}

	float GetCurrentClipLevel()
	{
		if (currentClip == nullptr) return 0.0f;
		return currentClip->GetClipLevel(animTime, currentClipMode);
	}
};