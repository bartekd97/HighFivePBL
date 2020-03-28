#include <iostream>

#include "ECSCore.h"
#include "LifeTimeSystem.h"
#include "LifeTime.h"

extern ECSCore gECSCore;

void LifeTimeSystem::Update(float dt)
{
	for (auto const& gameObject : gameObjects)
	{
		auto& lifeTime = gECSCore.GetComponent<LifeTime>(gameObject);
		lifeTime.lifeTime += dt;
		if (isgreater(lifeTime.threshold, 0.0f))
		{
			if (isgreaterequal(lifeTime.lifeTime, lifeTime.threshold))
			{
				lifeTime.lifeTime = 0.0f;
				std::cout << "[LifeTime] GameObject " << gameObject << " reached lifetime threshold (" << lifeTime.threshold << ")" << std::endl;
			}
		}
	}
}
