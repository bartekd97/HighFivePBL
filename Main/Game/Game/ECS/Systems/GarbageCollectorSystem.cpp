#include "GarbageCollectorSystem.h"
#include "../../HFEngine.h"

void GarbageCollectorSystem::Init()
{
	howManyFramesBetween = 1000;
	lastFrame = HFEngine::CURRENT_FRAME_NUMBER;
}

void GarbageCollectorSystem::PostUpdate(float dt) // TODO: make it only for debug mode
{
	if ((HFEngine::CURRENT_FRAME_NUMBER - lastFrame) < howManyFramesBetween)
	{
		return;
	}

	lastFrame = HFEngine::CURRENT_FRAME_NUMBER;
	std::vector<GameObject> toRemove;
	for (auto& gameObject : gameObjects)
	{
		auto pos = HFEngine::ECS.GetComponent<Transform>(gameObject).GetWorldPosition();
		if (!std::isfinite(pos.x) || !std::isfinite(pos.y) || !std::isfinite(pos.z))
		{
			toRemove.push_back(gameObject);
		}
	}
	for (auto& gameObject : toRemove)
	{
		LogInfo("[GarbageCollectorSystem] Detected bugged gameObject {}: {}", gameObject, HFEngine::ECS.GetNameGameObject(gameObject));
		HFEngine::ECS.DestroyGameObject(gameObject);
	}
}
