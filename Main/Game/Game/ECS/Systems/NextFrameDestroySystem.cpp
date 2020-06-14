#include "NextFrameDestroySystem.h"
#include "HFEngine.h"

void NextFrameDestroySystem::OnNextFrameDestroyRequested(Event& event)
{
	GameObject gameObject = event.GetParam<GameObject>(Events::GameObject::GameObject);
	requestedToNextFrameDestroy.insert(gameObject);
}

void NextFrameDestroySystem::Init()
{
	EventManager::AddListener(METHOD_LISTENER(Events::GameObject::NEXT_FRAME_DESTROY_REQUESTED, NextFrameDestroySystem::OnNextFrameDestroyRequested));
}

void NextFrameDestroySystem::Update(float dt)
{
	for (auto const& gameObject : requestedToNextFrameDestroy)
		HFEngine::ECS.DestroyGameObject(gameObject);

	requestedToNextFrameDestroy.clear();
}
