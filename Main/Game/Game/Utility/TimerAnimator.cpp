#include "TimerAnimator.h"

void TimerAnimator::Process(float dt)
{
	// process delayed actions
	for (int i = 0; i < delayedActions.size(); i++)
	{
		delayedActions[i].leftTime -= dt;
		if (delayedActions[i].leftTime <= 0.0f)
		{
			delayedActions[i].func();
			delayedActions.erase(delayedActions.begin() + i);
			i--;
		}
	}
}

void TimerAnimator::DelayAction(float time, std::function<void()> action)
{
	delayedActions.emplace_back(DelayedAction{ time, action });
}
