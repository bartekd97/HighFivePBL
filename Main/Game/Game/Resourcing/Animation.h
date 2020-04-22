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

class Animation
{
public:
	class Channel
	{
	public:
		static const int MAX_KEYS = 256;

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

	public:
		inline static std::shared_ptr<Channel> Create(
			std::vector<KeyVec3>& _positions,
			std::vector<KeyQuat>& _rotations,
			std::vector<KeyVec3>& _scales)
		{
			return std::shared_ptr<Channel>(new Channel(_positions, _rotations, _scales));
		}
	};

public:
	const float framerate = 0.0f;
	const float duration = 0.0f;
private:
	std::unordered_map<std::string, std::shared_ptr<Channel>> channels;

	Animation(float framerate, float duration)
		: framerate(framerate), duration(duration)
	{}
public:
	inline static std::shared_ptr<Animation> Create(float framerate, float duration) {
		return std::shared_ptr<Animation>(new Animation(framerate, duration));
	}

	inline void AddChannel(std::string name, std::shared_ptr<Channel> channel)
	{
		assert(channels.find(name) == channels.end() && "Channel already exists");
		channels[name] = channel;
	}
};