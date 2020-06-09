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

	// process updatings in time
	for (int i = 0; i < updatingsInTime.size(); i++)
	{
		updatingsInTime[i].dt += dt;
		float prog = updatingsInTime[i].dt / updatingsInTime[i].time;
		if (prog >= 1.0f)
		{
			updatingsInTime[i].func(1.0f);
			updatingsInTime.erase(updatingsInTime.begin() + i);
			i--;
		}
		else
		{
			updatingsInTime[i].func(prog);
		}
	}

	// process float animations
	float* _toDelete = NULL;
	for (auto& ffa : floatAnimations)
	{
		ffa.second.dt += dt;
		float p = ffa.second.dt / ffa.second.time;
		if (p >= 1.0f)
		{
			*ffa.first = ffa.second.to;
			_toDelete = ffa.first;
		}
		else
		{
			*ffa.first = glm::mix(ffa.second.from, ffa.second.to, p);
		}
	}
	if (_toDelete != NULL) floatAnimations.erase(_toDelete);
}

void TimerAnimator::DelayAction(float time, std::function<void()> action)
{
	delayedActions.emplace_back(DelayedAction{ time, action });
}

void TimerAnimator::UpdateInTime(float time, std::function<void(float)> update)
{
	updatingsInTime.emplace_back(UpdatingInTime{ time, 0.0f, update });
	update(0.0f);
}

void TimerAnimator::AnimateVariable(float* variable, float from, float to, float time)
{
	*variable = from;
	FloatAnimation fa;
	fa.from = from;
	fa.to = to;
	fa.time = time;
	fa.dt = 0.0f;
	floatAnimations[variable] = fa;
}

void TimerAnimator::AnimateVariable(glm::vec2* variable, glm::vec2 from, glm::vec2 to, float time)
{
	AnimateVariable(&(variable->x), from.x, to.x, time);
	AnimateVariable(&(variable->y), from.y, to.y, time);
}

void TimerAnimator::AnimateVariable(glm::vec3* variable, glm::vec3 from, glm::vec3 to, float time)
{
	AnimateVariable(&(variable->x), from.x, to.x, time);
	AnimateVariable(&(variable->y), from.y, to.y, time);
	AnimateVariable(&(variable->z), from.z, to.z, time);
}

void TimerAnimator::AnimateVariable(glm::vec4* variable, glm::vec4 from, glm::vec4 to, float time)
{
	AnimateVariable(&(variable->x), from.x, to.x, time);
	AnimateVariable(&(variable->y), from.y, to.y, time);
	AnimateVariable(&(variable->z), from.z, to.z, time);
	AnimateVariable(&(variable->w), from.w, to.w, time);
}
