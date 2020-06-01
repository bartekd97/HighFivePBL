#pragma once

#include <vector>
#include <functional>

class TimerAnimator
{
private:
	struct DelayedAction {
		float leftTime;
		std::function<void()> func;
	};
	std::vector<DelayedAction> delayedActions;

	TimerAnimator(TimerAnimator const&) = delete;
	void operator=(TimerAnimator const&) = delete;
public:
	TimerAnimator() = default;

	void Process(float dt);

	void DelayAction(float time, std::function<void()> action);
};