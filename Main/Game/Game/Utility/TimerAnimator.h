#pragma once

#include <vector>
#include <functional>
#include <unordered_map>
#include <glm/glm.hpp>

class TimerAnimator
{
private:
	struct DelayedAction {
		float leftTime;
		std::function<void()> func;
	};
	std::vector<DelayedAction> delayedActions;

	struct FloatAnimation {
		float from;
		float to;
		float time;
		float dt;
	};
	std::unordered_map<float*, FloatAnimation> floatAnimations;

	TimerAnimator(TimerAnimator const&) = delete;
	void operator=(TimerAnimator const&) = delete;
public:
	TimerAnimator() = default;

	void Process(float dt);

	void DelayAction(float time, std::function<void()> action);

	void AnimateVariable(float* variable, float from, float to, float time);
	void AnimateVariable(glm::vec2* variable, glm::vec2 from, glm::vec2 to, float time);
	void AnimateVariable(glm::vec3* variable, glm::vec3 from, glm::vec3 to, float time);
	void AnimateVariable(glm::vec4* variable, glm::vec4 from, glm::vec4 to, float time);
};