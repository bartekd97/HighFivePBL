#pragma once

#include <memory>
#include <unordered_map>
#include "Resourcing/AnimationClip.h"


// requires also SkinnedMeshRenderer
class SkinAnimator {
	friend class SkinAnimatorSystem;

private:
	struct StateSignature
	{
		uint16_t sTime;
		uint16_t sNextTime;
		uint16_t sTransitionDuration;
		bool sTransitioning;
		char* sCName;

		inline bool operator==(const StateSignature& other)
		{
			return (sTime == other.sTime
				&& sNextTime == other.sNextTime
				&& sTransitionDuration == other.sTransitionDuration
				&& sTransitioning == other.sTransitioning
				&& sCName == other.sCName);
		}
	};

	std::shared_ptr<AnimationClip> currentClip;
	AnimationClip::PlaybackMode currentClipMode;
	float animTime = 0.0f;

	std::shared_ptr<AnimationClip> nextClip;
	AnimationClip::PlaybackMode nextClipMode;
	float nextAnimTime = 0.0f;

	bool transitioning = false;
	float transitionDuration = 0.25f;

	StateSignature CalculateStateSignature()
	{
		StateSignature sign;
		sign.sTime = static_cast<uint16_t>(animTime * 1000.0f);
		sign.sNextTime = static_cast<uint16_t>(nextAnimTime * 1000.0f);
		sign.sTransitionDuration = static_cast<uint16_t>(transitionDuration * 1000.0f);
		sign.sTransitioning = transitioning;
		sign.sCName = (char *)currentClipName.c_str();
		return sign;
	}

	StateSignature lastSignature;
	float animatorSpeed = 1.0f;

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
		if (transitioning) {
			if (nextClip == nullptr) return 0.0f;
			return nextClip->GetClipLevel(nextAnimTime, nextClipMode);
		}
		else {
			if (currentClip == nullptr) return 0.0f;
			return currentClip->GetClipLevel(animTime, currentClipMode);
		}
	}

	void SetCurrentClipLevel(float level)
	{
		if (transitioning) {
			if (nextClip == nullptr) return;
			nextAnimTime = (nextClip->duration * level) / 1000.0f;
		}
		else {
			if (currentClip == nullptr) return;
			animTime = (currentClip->duration * level) / 1000.0f;
		}
	}

	float GetAnimatorSpeed()
	{
		return animatorSpeed;
	}
	void SetAnimatorSpeed(float speed)
	{
		animatorSpeed = speed;
	}
};