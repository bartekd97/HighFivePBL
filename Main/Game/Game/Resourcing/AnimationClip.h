#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <array>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

class AnimationClip
{
public:
	class Channel
	{
	public:
		static const int MAX_KEYS = 512;

		struct KeyVec3 {
			float time; // in ticks
			glm::vec3 key;
		};
		struct KeyQuat {
			float time; // in ticks
			glm::quat key;
		};

	private:
		int positionsCount;
		int rotationsCount;
		int scalesCount;
		std::array<KeyVec3, MAX_KEYS> positions{};
		std::array<KeyQuat, MAX_KEYS> rotations{};
		std::array<KeyVec3, MAX_KEYS> scales{};

		Channel(
			std::vector<KeyVec3>& _positions,
			std::vector<KeyQuat>& _rotations,
			std::vector<KeyVec3>& _scales)
		{
			assert(_positions.size() > 0 && _positions.size() <= MAX_KEYS && "Too many or none position keys");
			assert(_rotations.size() > 0 && _rotations.size() <= MAX_KEYS && "Too many or none rotation keys");
			assert(_scales.size() > 0 && _scales.size() <= MAX_KEYS && "Too many or none scale keys");
			positionsCount = _positions.size();
			rotationsCount = _rotations.size();
			scalesCount = _scales.size();
			std::copy_n(std::make_move_iterator(_positions.begin()), positionsCount, positions.begin());
			std::copy_n(std::make_move_iterator(_rotations.begin()), rotationsCount, rotations.begin());
			std::copy_n(std::make_move_iterator(_scales.begin()), scalesCount, scales.begin());
		}


		template<typename TKey, typename TRet>
		inline TRet Evaluate(TKey* arr, int count, float time, TRet (*func)(TRet const&, TRet const&, float))
		{
			if (count == 1) {
				return arr[0].key;
			}

			int index = count - 2;
			for (int i = 0; i < count - 1; i++) {
				if (time < arr[i + 1].time) {
					index = i;
					break;
				}
			}
			int nextIndex = index + 1;

			float dt = arr[nextIndex].time - arr[index].time;
			float factor = (time - arr[index].time) / dt;
			const TRet& start = arr[index].key;
			const TRet& end = arr[nextIndex].key;
			return func(start, end, factor);
		}

	public:
		inline static std::shared_ptr<Channel> Create(
			std::vector<KeyVec3>& _positions,
			std::vector<KeyQuat>& _rotations,
			std::vector<KeyVec3>& _scales)
		{
			return std::shared_ptr<Channel>(new Channel(_positions, _rotations, _scales));
		}

		inline glm::vec3 EvaluatePosition(float time)
		{
			return Evaluate<KeyVec3, glm::vec3>(positions.data(), positionsCount, time, glm::mix);
		}

		inline glm::quat EvaluateRotation(float time)
		{
			return Evaluate<KeyQuat, glm::quat>(rotations.data(), rotationsCount, time, glm::slerp);
		}

		inline glm::vec3 EvaluateScale(float time)
		{
			return Evaluate<KeyVec3, glm::vec3>(scales.data(), scalesCount, time, glm::mix);
		}

	};

public:
	const float framerate = 0.0f;
	const float duration = 0.0f;
private:
	std::unordered_map<std::string, std::shared_ptr<Channel>> channels;

	AnimationClip(float framerate, float duration)
		: framerate(framerate), duration(duration)
	{}
public:
	inline static std::shared_ptr<AnimationClip> Create(float framerate, float duration) {
		return std::shared_ptr<AnimationClip>(new AnimationClip(framerate, duration));
	}

	inline void AddChannel(std::string name, std::shared_ptr<Channel> channel)
	{
		assert(channels.find(name) == channels.end() && "Channel already exists");
		channels[name] = channel;
	}

	inline glm::mat4 EvaluateChannel(std::string name, float timeInSeconds, glm::mat4& fallback)
	{
		if (!channels.contains(name))
			return fallback;

		float timeInTicks = timeInSeconds * 1000.0f; // to ms
		float time = glm::mod(timeInTicks, duration);
		//time = 0.0f;

		auto& channel = channels[name];
		glm::vec3 position = channel->EvaluatePosition(time);
		glm::quat rotation = channel->EvaluateRotation(time);
		glm::vec3 scale = channel->EvaluateScale(time);

		//return fallback;
		return glm::translate(glm::mat4(1.0), position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0), scale);
	}
};